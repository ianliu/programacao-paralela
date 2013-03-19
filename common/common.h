#ifndef __COMMON_H__
#define __COMMON_H__

/**
 * Fills the @sendcnts and @displs vectors to scatter the data.
 */
void get_scatter_info(int *sendcnts, int *displs, int n_ranks, int n);

#endif /* end of include guard: __COMMON_H__ */

