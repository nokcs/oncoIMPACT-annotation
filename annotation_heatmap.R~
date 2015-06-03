setwd("/home/nok/workspace/oncoIMPACT-annotation/")
library("gplots")


pdf(file="20drivers_20annnotations_GBM.pdf",
	paper="special",
	width=20,
	height=20
	)

#c(bottom, left, top, right) 
par(oma = c(25,5,5,5)) #Outer Margin Area: all sides have 5 lines of space
par(mar = c(5,5,5,5)) #the figure margins
par(lwd = 2)  #The line width
#palette <- colorRampPalette(c('#f0f3ff','#0033BB'))(10) #10 colors
palette <- colorRampPalette(c('#00CC00','#000000'))(10) #10 colors

#read input
profile <- read.table("annotation_matrix_20drivers_20annnotations_GBM.dat", header = TRUE)
profile_mat = as.matrix(profile)
numDataset <- 1
numTopDrivers <- 20

#set up clustering function
hclustfunc <- function(x) hclust(x, method="complete")
distfunc <- function(x) dist(x, method="euclidean")
cl.row <- hclustfunc(distfunc(profile_mat))
cl.col <- hclustfunc(distfunc(t(profile_mat)))

#gr.row <- cutree(cl.row, 7)
#gr.col <- cutree(cl.col, 5)
#require(RColorBrewer)
#col1 <- brewer.pal(7, "Set1")
#col2 <- brewer.pal(5, "Pastel1")


#http://www.r-bloggers.com/color-palettes-in-r/
col = rainbow(numDataset)
rowCol <- c()
for (i in 1:numDataset){
  rowCol <- c(rowCol, rep(col[i], numTopDrivers))
}


heatmap.2(profile_mat,
	  main = "", 
	  scale="none",
	  #col="terrain.colors",
    col=palette,
	  
	  #For the colors
	  #ColSideColors = c("purple","purple","purple","purple","darkblue","darkblue","darkblue","darkred","darkred","darkred"),
	  #RowSideColors = c("purple","purple","purple","purple","darkblue","darkblue","darkblue","darkred","darkred","darkred"),
	  
	  #RowSideColors = col1[gr.row],
	  #RowSideColors = rowCol,
    
    #Rowv=FALSE,Colv=FALSE,
	  #breaks = c(0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1), #symbreaks = TRUE,

    #key.par=list(mgp=c(1.5, 0.5, 0),
    #mar=c(2.5, 2.5, 1, 0)),
	  
	  #For the tree
	  dendrogram = "row", 
	  hclustfun = hclustfunc,
	  distfun = distfunc,
	  key = FALSE,
    #lmat=rbind(c(2),c(3),c(1),c(3)), 
    #lhei=c(1,1,9,0), 
    #lwid=c(1),
    #cellnote=as.matrix(profile_mat),notecol="black",notecex=4,
	  margins =c(5,5),  #row, column
    #cexRow=4, cexCol=4, #The magnification to be used for axis annotation
	  density.info="none", 
    trace="none"  #character string indicating whether a solid "trace" line should be drawn
)

dev.off()