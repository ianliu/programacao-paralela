package org.myorg;

import java.io.BufferedReader;
import java.io.DataInput;
import java.io.DataOutput;
import java.io.EOFException;
import java.io.FileReader;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.ArrayWritable;
import org.apache.hadoop.io.FloatWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.WritableComparable;
import org.apache.hadoop.io.compress.CompressionCodecFactory;
import org.apache.hadoop.mapred.FileInputFormat;
import org.apache.hadoop.mapred.FileOutputFormat;
import org.apache.hadoop.mapred.FileSplit;
import org.apache.hadoop.mapred.InputSplit;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.JobConfigurable;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.RecordReader;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;
import org.apache.hadoop.mapred.TextInputFormat;
import org.apache.hadoop.mapred.TextOutputFormat;
import org.apache.hadoop.util.StringUtils;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

public class ImageCondition extends Configured implements Tool {

    public static class Map
            extends MapReduceBase
            implements Mapper<LongWritable, FloatArrayWritable, IntWritable, FloatWritable>
    {
        private int nz;
        private int nx;
        private boolean big_endian = false;
        String inputFile;
        RandomAccessFile source;

        public void configure(JobConf conf)
        {
            nz = conf.getInt("imagecond.size.nz", -1);
            nx = conf.getInt("imagecond.size.nx", -1);
            big_endian = conf.getBoolean("imagecond.big_endian", false);
            inputFile = conf.get("map.input.file");

            try {
                Path[] files = DistributedCache.getLocalCacheFiles(conf);
                Path src = files[0];
                source = new RandomAccessFile(src.toString(), "r");
            } catch (IOException ioe) {
                System.err.println("Caught exception while getting cached files: "
                        + StringUtils.stringifyException(ioe));
            }
        }

        private float[] getSourceFrame(long offt) throws IOException {
            float[] v = new float[nz*nx];
            long frame = offt/(nz*nx);
            source.seek(frame);
            try {
                for (int i = 0; i < nz*nx; i++) {
                    int k = source.readInt();
                    if (!big_endian)
                        k = Integer.reverseBytes(k);
                    v[i] = Float.intBitsToFloat(k);
                }
            } catch (EOFException e) {
                return null;
            }
            return v;
        }

        @Override
        public void map(LongWritable key, FloatArrayWritable value,
                OutputCollector<IntWritable, FloatWritable> output,
                Reporter reporter)
            throws IOException
        {
            float[] frame = getSourceFrame(key.get());
            FloatWritable[] v = (FloatWritable[]) value.get();
            for (int i = 0; i < v.length; i++)
                output.collect(new IntWritable(i), new FloatWritable(frame[i]*v[i].get()));
        }
    }

    public static class Reduce
            extends MapReduceBase
            implements Reducer<IntWritable, FloatWritable, IntWritable, FloatWritable>
    {
        @Override
        public void reduce(IntWritable key, Iterator<FloatWritable> values,
                OutputCollector<IntWritable, FloatWritable> output,
                Reporter reporter)
            throws IOException
        {
            float sum = 0;
            while (values.hasNext()) {
                sum += values.next().get();
            }
            output.collect(key, new FloatWritable(sum));
        }
    }

    public int run(String[] args) throws Exception {
        JobConf conf = new JobConf(getConf(), ImageCondition.class);
        conf.setJobName("image-condition");

        conf.setOutputKeyClass(IntWritable.class);
        conf.setOutputValueClass(FloatWritable.class);

        conf.setMapperClass(Map.class);
        conf.setReducerClass(Reduce.class);

        conf.setInputFormat(CubeInputFormat.class);
        conf.setOutputFormat(TextOutputFormat.class);

        int i;
        for (i = 0; i < args.length; i++) {
            if (args[i].equals("-s") || args[i].equals("--source")) {
                DistributedCache.addCacheFile(new Path(args[++i]).toUri(), conf);
            } else if (args[i].equals("-r") || args[i].equals("--receptors")) {
                FileInputFormat.setInputPaths(conf, new Path(args[++i]));
            } else if (args[i].equals("-o") || args[i].equals("--output")) {
                FileOutputFormat.setOutputPath(conf, new Path(args[++i]));
            } else if (args[i].equals("--nz")) {
                conf.setInt("imagecond.size.nz", Integer.parseInt(args[++i]));
            } else if (args[i].equals("--nx")) {
                conf.setInt("imagecond.size.nx", Integer.parseInt(args[++i]));
            }
        }

        JobClient.runJob(conf);
        return 0;
    }

    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(), new ImageCondition(), args);
        System.exit(res);
    }
}

class CubeInputFormat
    extends FileInputFormat<LongWritable, FloatArrayWritable>
    implements JobConfigurable
{
    private CompressionCodecFactory compressionCodecs = null;

    public void configure(JobConf conf) {
        compressionCodecs = new CompressionCodecFactory(conf);
    }

    protected boolean isSplitable(FileSystem fs, Path file) {
        return compressionCodecs.getCodec(file) == null;
    }

    public RecordReader<LongWritable, FloatArrayWritable>
        getRecordReader(InputSplit genericSplit, JobConf job, Reporter reporter)
        throws IOException
    {
        reporter.setStatus(genericSplit.toString());
        int nz = job.getInt("imagecond.size.nz", -1);
        int nx = job.getInt("imagecond.size.nx", -1);
        return new CubeRecordReader(job, (FileSplit) genericSplit, nz*nx);
    }
}

class CubeRecordReader implements RecordReader<LongWritable, FloatArrayWritable> 
{
    private long start;
    private long pos;
    private long end;
    private int size;
    private boolean bigEndian = false;
    private FSDataInputStream in;

    private final int DATA_SIZE = Float.SIZE / 8;

    public CubeRecordReader(Configuration job, FileSplit split, int size)
        throws IOException
    {
        this.size = size;
        this.start = calculateStart(split);
        this.pos = start;
        this.end = calculateEnd(split);
        final Path file = split.getPath();
        this.in = file.getFileSystem(job).open(file);
    }

    private long calculateStart(FileSplit split) {
        long s = split.getStart() / DATA_SIZE;
        return (long)(Math.floor((double)(s - 1) / size) + 1) * size * DATA_SIZE;
    }

    private long calculateEnd(FileSplit split) {
        long s = (split.getStart() + split.getLength()) / DATA_SIZE;
        return (s / size + 1) * size * DATA_SIZE;
    }

    public LongWritable createKey() {
        return new LongWritable();
    }

    public FloatArrayWritable createValue() {
        return new FloatArrayWritable();
    }

    public synchronized boolean next(LongWritable key, FloatArrayWritable value)
        throws IOException
    {
        if (pos >= end)
            return false;

        key.set(pos / DATA_SIZE);
        FloatWritable[] v = new FloatWritable[size];

        int read;
        for (read = 0; read < size; read++) {
            int k;
            try {
                k = in.readInt();
            } catch (EOFException e) {
                break;
            }
            if (!bigEndian)
                k = Integer.reverseBytes(k);
            v[read] = new FloatWritable(Float.intBitsToFloat(k));
        }

        if (read == 0)
            return false;

        FloatWritable[] result;

        if (read < size)
            result = Arrays.copyOf(v, read);
        else
            result = v;

        value.set(result);
        pos += read * DATA_SIZE;

        return true;
    }

    public float getProgress() {
        if (start == end) {
            return 0.0f;
        } else {
            return Math.min(1.0f, (pos - start) / (float)(end - start));
        }
    }

    public synchronized long getPos() throws IOException {
        return pos;
    }

    public synchronized void close() throws IOException {
        if (in != null)
            in.close();
    }
}

class FloatArrayWritable extends ArrayWritable {
    public FloatArrayWritable() {
        super(FloatWritable.class);
    }

    public FloatArrayWritable(FloatWritable[] values) {
        super(FloatWritable.class, values);
    }

    public String toString() {
        StringBuilder sb = new StringBuilder();
        for (String s : super.toStrings())
            sb.append(s).append(" ");
        return sb.toString();
    }
}
