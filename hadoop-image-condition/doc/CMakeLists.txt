project(HadoopImgCondDoc NONE)
include(UseLATEX)
add_latex_document(main.tex
  IMAGES images/cubo-de-dados.eps images/solo.eps
	DEFAULT_PDF)
