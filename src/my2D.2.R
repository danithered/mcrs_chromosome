library(RRNA)
library(ggplot2)
library(ggplotify)
#library(grid)
library(cowplot)
library(plotrix)

addToPlot <- function(x=0, y=0, coords, 
                      xspan=1, rot = NA, gap=0.05, 
                      main_con=NA, side_con=NA, 
                      add_letter=F, cex_letter=par("cex"), col_letter="black", col=NULL,
                      ...){
  #rotate
  if(!is.na(rot)){
    midx <- mean(coords$x)
    midy <- mean(coords$y)
    dist <- sqrt(coords$x^2 + coords$y^2)
    angle <- atan2(coords$y, coords$x) + rot
    coords$y <- midy + sin(angle) * dist
    coords$x <- midx + cos(angle) * dist
    
  }
  
  #align
  coords$x <- (coords$x - min(coords$x))
  coords$y <- (coords$y - min(coords$y))
  
  #rescale
  scaling=1
  if(!is.na(xspan)){
    #compute scaling factor
    scaling = xspan/(max(coords$x) - min(coords$x))
    
    coords$x <- coords$x * scaling
    coords$y <- coords$y * scaling
  }
  
  #realign
  coords$x <- x + coords$x
  coords$y <- y + coords$y
  
  #connector lines
  if(!is.na(main_con)){
    if(is.logical(main_con)) if(main_con) main_con <- list(lwd=1, lty=1, col="black")
    
    with(main_con, {
      #points(coords$x, coords$y, type="c", col=col, lwd=lwd, lty=lty)
      segments(coords[1:(nrow(coords)-1),"x"], 
               coords[1:(nrow(coords)-1),"y"], 
               coords[2:nrow(coords),"x"], 
               coords[2:nrow(coords),"y"], 
               col=col, lwd=lwd, lty=lty)
    })
    
  }
  
  if(!is.na(side_con)){
    if(is.logical(side_con)) if(side_con) side_con <- list(lwd=1, lty=1, col="black")
    
    with(side_con, {
      segs <- data.frame()
      
      for(base in 1:nrow(coords)){
        if(coords[base, "bound"] > 0 & base < coords[base, "bound"]){
          segs <- rbind(segs, data.frame(
            x0=coords[base, "x"], 
            y0=coords[base, "y"],
            x1=coords[coords[base, "bound"], "x"],
            y1=coords[coords[base, "bound"], "y"]  ))
        }
      }
      segments(segs$x0, segs$y0, segs$x1, segs$y1, lwd=lwd, col=col, lty=lty)
      
    })
    
  }
  
  #points(coords$x, coords$y)
  # symbols(coords$x, coords$y, 
  #         circles=rep(scaling* (1/2-gap), nrow(coords)), 
  #         inches=F, add=T, ...)
  if(is.null(col)){
  	cols <- rep("transparent", nrow(coords))
  } else {
  	cols <- col
  }
  for(ci in 1:nrow(coords) ) draw.circle(coords$x[ci], coords$y[ci],
  						radius = scaling* (1/2-gap), nrow(coords),
  						col=cols[ci],
  						...)
  #add letter
  lett <- coords[add_letter, ]
  if(nrow(lett) > 0) {
    text(lett$x, lett$y, labels=lett$seq, cex=cex_letter, col=col_letter)
  }
  
  #return(coords)
}

#testing add: 19(cra) 22(cxy) 23(din) 25(family) 28(fin) 48(mgp) 56(pin) 57(plt) 64(usr) 68(xpd)
what = names(par())[56]
#par(mfrow=c(1,1))
{
#par(fin=c(5,5))
plot(0, 0, asp=1, xlim=c(0, 1), ylim=c(0,1), main= paste(par(what), collapse = " "))
addToPlot(0,0, coords, main_con=T, xspan = 1)
plot_grid(as.ggplot(function(x){
	#par(mgp=c(3,1,0))
	plot(0, 0, asp=1, xlim=c(0, 1), ylim=c(0,1), main= paste(par(what), collapse = " ") )
	#plot.new()
	#plot.window(asp=1, ylim=c(0,1), xlim=c(0, 1))
	addToPlot(0,0, coords, main_con=T, xspan = 1)
#}), scale=1.2)
}, scale=1))
}



dirto <- function(to, from=c(0,0)){
	
	return(atan2(to[2] - from[2], to[1] - from[1]) / pi)
}
rad2deg <- function(rad) {rad * 180 / pi}
pirad2deg <- function(rad) {rad * 180}
deg2rad <- function(deg) {deg * pi / 180}
deg2pirad <- function(deg) {deg / 180}

setwd("/home/danielred/data/programs/mcrs_chromosome/IN/str/") 
strID <- list.dirs(recursive = F, full.names = F)  


pooldata=list()
for(id in strID) {
  # process rule
  rule <- readLines(paste(id, "str.txt", sep="/"))
  subs <- grep(".", rule, fixed=T)
  if(length(subs) > 1){ rule <- rule[ 1:(subs[2]-1) ] } #ignore subrules > 1
  
  #filecon <- file(paste(id, rev(list.files(id, pattern="randseqs_ea*"))[1], sep="/"))
  poolfile <- read.table(paste(id, rev(list.files(id, pattern="randseqs_ea*"))[1], sep="/"))
  colnames(poolfile) <- c("seq", 
													"str", 
													"mfe", 
													"Pfold", 
													"Pdeg", 
													"no_sites",
													"R",
													"length",
													"type",
													paste0("a", 0:(ncol(poolfile)-10)) 
													)
  
  #open(filecon)
  startofPattern <- NA
  linecount = 0
  while(is.na(startofPattern)){
  	#read in next line
  	linecount = linecount+1
  	repl <- poolfile[linecount, c("seq", "str")]
    #repl <- strsplit(line, "\t", fixed=T)[[1]][1:2]
  	
    where <- gregexpr(rule[1], repl[2], fixed=T)[[1]]
    if(where[1] > -1){ #it has this activity
      #check for subrules
      for(w in where){ #check every possible location
        #check subrules
        found = T
        for(sr in rule[2:length(rule)]){
          sub <- strsplit(sr, " ")[[1]]
          if(substr(repl[1],
          					w + as.numeric(sub[1])-1,
          					w + as.numeric(sub[1])-1 ) != sub[2]) { #it is not ok 
            found = F
            break
          }
        } # going tru subrules
        if(found){ #everything is ok
          startofPattern = w
          break
        }
      } # checking possible locations
    }
  } # looping tru file to find ok replicator
  #close(filecon)
  
  if( is.na(startofPattern) ) {
    cat(paste("ERROR: no pattern found in", id, "\n"))
    return(NA)
  }
  
  mask <- paste(rep("N", nchar(repl[1])), collapse="")
  for(sr in rule[2:length(rule)]){
    sub <- strsplit(sr, " ")[[1]]
    ch = startofPattern + as.numeric(sub[1]) -1
    #substr(mask, ch, ch) <- sub[2]
    substr(mask, ch, ch) <- substr(repl[1], ch, ch)
  }
  
  fr = read.table(paste(id, rev(list.files(id, pattern="randseqs_ea*"))[1], sep="/"))
  colnames(fr) <- c("seq", 
                    "str",
                    "mfe", 
                    "Pfold",
                    "Pdeg",
                    "no_sites",
                    "R",
                    "length", #place of M
                    "type",
                    paste0("a", 0:(ncol(fr)-10)))
  
  
  pooldata[[id]] <- list(id=id,
						             seq=as.character(repl[1]), 
						             str=as.character(repl[2]), 
						             start=startofPattern, 
						             end=startofPattern + nchar(rule[1]) - 1,
						             mask=mask,
						             n = nrow(fr),
						             #length
						             mean_length=mean(fr$length),
						             sd_length=sd(fr$length),
						             #replicability
						             mean_R=mean(fr$R),
						             sd_R=sd(fr$R),
						             #Pdeg
						             mean_Pdeg=mean(fr$Pdeg),
						             sd_Pdeg=sd(fr$Pdeg),
						             #Pfold
						             mean_Pfold=mean(fr$Pfold),
						             sd_Pfold=sd(fr$Pfold),
						             #MFE
						             mean_mfe=mean(fr$mfe),
						             sd_mfe=sd(fr$mfe)
        						)
}

#pooldata <- pooldatas[[1]]##noo
p.colnames <- names(pooldata[[1]])
pooldata = do.call(rbind.data.frame, pooldata)
colnames(pooldata) <- p.colnames

r=1

#plot(0,0, asp=1)
ct=makeCt( pooldata$str[r], pooldata$seq[r])
coords=ct2coord(ct)

seqrule <- strsplit(pooldata$mask[r], "")[[1]] != "N"

colormask <- rep(NA, length(seqrule))
colormask[ pooldata[r, "start"]:pooldata[r, "end"] ] <- "red" 
colormask[ seqrule ] <- "coral" 
colormask[pooldata[r, "start"] + which(strsplit(pooldata[r, "str"], "")[[1]][ pooldata[r, "start"]:pooldata[r, "end"] ] != ".") -1] <- "blue"

p1 <- as.ggplot(~{
	plot.new()
	plot.window(asp=1, xlim=c(0,1), ylim=c(0,1), xpd=NA)
	addToPlot(0,0,coords, xspan=1,
						fg="lightblue",
						#bases
						add_letter = seqrule,
						cex_letter = 0.5,
						col_letter = "red",
						#fill
						bg=colormask,
						#rotate it
						#rot=pi/2,
						#connecting lines
						main_con = T
						#, side_con = list(lwd=1, col="red", lty=1)
						)
})
p2 <- as.grob(~barplot(1:2))
p1
p3 <- as.ggplot(~plot(0,0,asp=1))
plot_grid(p1, p3, ncol=1, greedy = F, scale=c(2,1))


#plot properties of pool
maxok <- sapply(pooldata[, c("n", "mean_length", "mean_R", "mean_mfe")], max, na.rm=T)
minek <- sapply(pooldata[, c("n", "mean_length", "mean_R", "mean_mfe")], max, na.rm=T)

barplot( unlist(pooldata[r, c("mean_Pdeg", "mean_Pfold")]), 
				 xlim=c(0,7), 
				 axes=F, 
				 ylim=c(-1,1)
)
barplot( c(rep(NA, 2), pooldata[r,"n"] / maxok["n"] ), add=T, axes=F )
barplot( c(rep(NA, 3), pooldata[r,"mean_length"] / maxok["mean_length"] ), add=T, axes=F )
barplot( c(rep(NA, 4), pooldata[r,"mean_R"] / maxok["mean_R"] ), add=T, axes=F )
barplot( c(rep(NA, 5), -pooldata[r,"mean_mfe"] / maxok["mean_mfe"] ), add=T, axes=F )
text()

Rmax=50
lmax = 100

pfl <- stack(poolfile[,c("mfe", "Pfold", "Pdeg", "R", "length")])
ggplot(data=pfl, aes(x=1, y=values)) +
	facet_grid(ind~.)+
	#geom_boxplot(aes(y=values))+
	geom_violin(width=1)+
	stat_summary(fun=mean, geom="point")
	coord_cartesian(ylim = c(0, -30))
	scale_x_discrete(labels=parse( text= levels(stlong$type) ), drop=drop_unused)+
	labs(title= tt, subtitle=whattosee)+
	theme(axis.title.x = element_blank(), 
				axis.text.x=element_blank(),
				axis.ticks = element_blank(),
				legend.position = "none",
				axis.title.y = element_blank())

tulplots <- list(
	ggplot(data=poolfile, aes(x=1, y=mfe)) +
		geom_violin(width=1)+
		stat_summary(fun=mean, geom="point")+
		coord_cartesian(ylim = c(0, -30))+
		theme(axis.title.x = element_blank(), 
					axis.text.x=element_blank(),
					axis.ticks = element_blank(),
					legend.position = "none")
,	
	ggplot(data=poolfile, aes(x=1, y=Pfold)) +
		geom_violin(width=1)+
		stat_summary(fun=mean, geom="point")+
		coord_cartesian(ylim = c(0, 1))+
		scale_y_continuous(breaks=c(0,1), labels = c("0%", "100%"))+
		theme(axis.title.x = element_blank(), 
					axis.text.x=element_blank(),
					axis.ticks = element_blank(),
					legend.position = "none")
,	
	ggplot(data=poolfile, aes(x=1, y=Pdeg)) +
		geom_violin(width=1)+
		stat_summary(fun=mean, geom="point")+
		scale_y_continuous(breaks=c(0,1), labels = c("0%", "100%"))+
		coord_cartesian(ylim = c(0, 1))+
		theme(axis.title.x = element_blank(), 
					axis.text.x=element_blank(),
					axis.ticks = element_blank(),
					legend.position = "none")
,	
	ggplot(poolfile, aes(x=1, y=R)) +
		geom_violin(width=1)+
		stat_summary(fun=mean, geom="point")+
		coord_cartesian(ylim = c(0, Rmax))+
		theme(axis.title.x = element_blank(), 
					axis.text.x=element_blank(),
					axis.ticks = element_blank(),
					legend.position = "none")
,	
	ggplot(data=poolfile, aes(x=1, y=length)) +
		geom_violin(width=1)+
		stat_summary(fun=mean, geom="point")+
		coord_cartesian(ylim = c(0, lmax))+
		theme(axis.title.x = element_blank(), 
					axis.text.x=element_blank(),
					axis.ticks = element_blank(),
					legend.position = "none")
,
	ggplot(data=data.frame(n= nrow(poolfile) ), aes(y=n, x=1)) +
		geom_bar(stat = "identity")+
		theme(axis.title.x = element_blank(), 
					axis.text.x=element_blank(),
					axis.ticks = element_blank(),
					legend.position = "none") +
		coord_cartesian(ylim=c(0, all))+
		scale_y_continuous(breaks = c(0, all), 
											 labels = c("0%", "100%"))
,
	as.ggplot(~{
		plot.new()
		plot.window(asp=1, xlim=c(0,1), ylim=c(0,1), xpd=NA)
		addToPlot(0,0,coords, 
							xspan=1,
							border="lightblue",
							#bases
							add_letter = seqrule,
							cex_letter = 0.5,
							col_letter = "red",
							#fill
							col=colormask,
							#rotate it
							#rot=pi/2,
							#connecting lines
							main_con = T
							#, side_con = list(lwd=1, col="red", lty=1)
		)
	}, scale=1)
)
plot_grid(plotlist=tulplots, ncol=1, align="v")
plot_grid( tulplots[[7]], ncol=1, align="v")
plot_grid(plotlist=lapply(tulplots, function(x) x + theme(axis.title.y = element_blank(), axis.text.y = element_blank())))

numbers <- data.frame(id=c("1", "2"), n=c(1000, 104))
all <- 3000
ggplot(numbers, aes(x=id, y=n))+
	geom_bar(stat="identity")+
	scale_y_continuous(breaks = c(0, all), 
										 labels = c("0%", "100%"), 
										 limits=c(0, all)
										 )
all=50000





#copied
		#count
		ggplot(data=stlong[stlong$ind=="count",], aes(x=type, 
																									fill=type,
																									#y=values / as.numeric(p$par_ncol) / as.numeric(p$par_nrow)
																									y=values / sum(stlong[stlong$ind=="count", "values"])
		)) +
			facet_grid(ind~.)+
			geom_bar(stat="identity") +
			geom_text(aes(label=values), 
								position = position_stack(vjust=0.5),
								size=2, 
								fill=NA, 
								angle=270)+
			scale_x_discrete(labels = parse_format(), drop=drop_unused )+
			scale_y_continuous(breaks= c(0, 1), 
												 labels= c("0%", "100%"))+
			coord_cartesian(ylim = c(0, 1), clip="off" )+
			theme(legend.position = "none",
						axis.ticks = element_blank(),
						axis.title.y = element_blank())













































numbs <- read.table("numbers.txt")
numbs <- numbs[1:(nrow(numbs)-1),]
#numbs[sapply(strsplit(numbs$V2, "/"), function(x) return(as.numeric(x[2]))), ]
numbs$V2 <-  sapply(strsplit(numbs$V2, "/"), function(x) return( as.numeric(x[2]) ))

numbs <- numbs[order(numbs$V2), ]

setwd("~/str")
seq <- c()
seq <-sapply(numbs$V2 , function(x) {
	temp <- readLines(paste0(x, "/str.txt"))[2:4]
	n <- readLines(paste0(x, "/str.txt"))[1]
	out <- paste0(rep("N", nchar(n)), collapse = "")
	for(t in temp) 
		substring(out, 
					 as.numeric(strsplit(t, " ")[[1]][1]),
					 as.numeric(strsplit(t, " ")[[1]][1])) <- strsplit(t, " ")[[1]][2]
	return(out)
	})

str <- c()
for(n in numbs$V2) str <- c(str, readLines(paste0(n, "/str.txt"))[1])
numbs <- cbind(numbs, str, seq)

{
	par(mfrow=c(2,1), mar=c(0, 0.5,0,0.5), oma=c(0,1,2,0))
	
	barplot(numbs$V1, xlim=c(0,1.5*nrow(numbs)), axes=F, width=1.34, space=0.1)
	
	
	plot.new( )
	plot.window( xlim=c(0,1.5*nrow(numbs)), ylim=c(0,0.4), asp=1 )
	x_orig=0
	
	for(nth in numbs$V2) {
		str=numbs[nth, "str"]
		seq=numbs[nth, "seq"]
		strsplit <- strsplit(str, "")[[1]]
		if(strsplit[length(strsplit)] == "("){
			str=paste0(str, "...)..)")
			seq=paste0(seq, "NNNNNNN")
			ct=makeCt( str, seq)
			coords=ct2coord(ct)
			coords <- coords[coords$num %in% 1:(max(coords$num-7)), ]
		} else {
			ct=makeCt( str, seq)
			coords=ct2coord(ct)
		}
		length=nchar(numbs[nth, "str"])
		addToPlot(x_orig,1.8,coords, xspan=1, 
							add_letter = strsplit(numbs[nth, "seq"], "")[[1]] != "N", 
							cex_letter = 1, 
							col_letter = "red",
							fg="lightblue", 
							# bg=NA,
							bg=c(rep("lightblue", 1),
									 rep("red", nrow(coords)/2-2 ),
									 rep("coral", 3),
									 rep("red", nrow(coords)/2-2 ),
									 rep("lightblue", 1)),
							rot=pi/2,
							main_con = T, side_con = list(lwd=1, col="red", lty=1) )
		print(x_orig)
		
		x_orig = x_orig+1.5
	}
}















# with library
ct=makeCt( "((((...(((((((....)))))))...((((...))))...))))",
					 "CCCCAAAGGGGGGGAUUACCCCUCCUUUAAAAGGGUUUUCCCCCCC"
)
coords=ct2coord(ct)
RNAPlot(coords)

plot(coords$x, coords$y, asp=1)

addToPlot <- function(x=0, y=0, coords, 
											xspan=1, rot = NA, gap=0.05, 
											main_con=NA, side_con=NA, 
											add_letter=F, cex_letter=par("cex"), col_letter="black",
											...){
	#rotate
	if(!is.na(rot)){
		midx <- mean(coords$x)
		midy <- mean(coords$y)
		dist <- sqrt(coords$x^2 + coords$y^2)
		angle <- atan2(coords$y, coords$x) + rot
		coords$y <- midy + sin(angle) * dist
		coords$x <- midx + cos(angle) * dist
		
	}
	
	#align
	coords$x <- (coords$x - min(coords$x))
	coords$y <- (coords$y - min(coords$y))
	
	#rescale
	scaling=1
	if(!is.na(xspan)){
		#compute scaling factor
		scaling = xspan/(max(coords$x) - min(coords$x))
		
		coords$x <- coords$x * scaling
		coords$y <- coords$y * scaling
	}
	
	#realign
	coords$x <- x + coords$x
	coords$y <- y + coords$y
	
	#connector lines
	if(!is.na(main_con)){
		if(is.logical(main_con)) if(main_con) main_con <- list(lwd=1, lty=1, col="black")
		
		with(main_con, {
			#points(coords$x, coords$y, type="c", col=col, lwd=lwd, lty=lty)
			segments(coords[1:(nrow(coords)-1),"x"], 
							 coords[1:(nrow(coords)-1),"y"], 
							 coords[2:nrow(coords),"x"], 
							 coords[2:nrow(coords),"y"], 
							 col=col, lwd=lwd, lty=lty)
		})
		
	}
	
	if(!is.na(side_con)){
		if(is.logical(side_con)) if(side_con) side_con <- list(lwd=1, lty=1, col="black")
		
		with(side_con, {
			segs <- data.frame()
			
			for(base in 1:nrow(coords)){
				if(coords[base, "bound"] > 0 & base < coords[base, "bound"]){
					segs <- rbind(segs, data.frame(
						x0=coords[base, "x"], 
						y0=coords[base, "y"],
						x1=coords[coords[base, "bound"], "x"],
						y1=coords[coords[base, "bound"], "y"]  ))
				}
			}
			segments(segs$x0, segs$y0, segs$x1, segs$y1, lwd=lwd, col=col, lty=lty)
			
		})
		
	}
	
	#points(coords$x, coords$y)
	symbols(coords$x, coords$y, 
					circles=rep(scaling* (1/2-gap), nrow(coords)), 
					inches=F, add=T, ...)
	
	#add letter
	lett <- coords[add_letter, ]
	if(nrow(lett) > 0) {
		text(lett$x, lett$y, labels=lett$seq, cex=cex_letter, col=col_letter)
	}
	
	#return(coords)
}


plot(0,0, xlim=c(-2,2), ylim=c(-2,2), asp=1)
coords <- coords[1:15,]
addToPlot(-2,-2,coords, xspan=3, add_letter = c(T, F), fg="blue", bg="blue", cex_letter = 0.5, col_letter = "red", rot=pi/2)


plot(1:2,c(1,1), cex=1, xlim = c(-5,5), ylim=c(-5,5))
plot(1:2,1:2, cex=1, xlim = c(-10,10), ylim=c(-10,10) )
par("pin")
symbols(1:2, c(1,1), circles=1, inches = F)
symbols(1:2, c(1,1), circles=rep(span/2, 2), inches=F, add=T)
symbols(x=1:2, y=c(1,1), circles=c(0.5,0.5),
				ann=F, bg="steelblue2", fg="blue", inches = F)

{
par(mfrow=c(2,1), mar=c(1, 0.5,0.5,0.5))
	
plot.new( )
plot.window( xlim=c(0,7.5), ylim=c(0,1.5), asp=1 )
x_orig=0

for(length in seq(5, 13, 2)) {
	ct=makeCt( paste0("((", paste0(rep(".", length), collapse = ""), "))"),
						 paste0(sample(c("A","C","G","U"), length+4, replace = T), collapse = "")
	)
	coords=ct2coord(ct)
	addToPlot(x_orig,0,coords, xspan=1, 
						add_letter = ((length+4)/2-0.5):((length+4)/2+1.5), 
						cex_letter = 1, 
						col_letter = "red",
						fg="lightblue", 
						# bg=NA,
						bg=c(rep("lightblue", 1),
								 rep("red", nrow(coords)/2-2 ),
								 rep("coral", 3),
								 rep("red", nrow(coords)/2-2 ),
								 rep("lightblue", 1)),
						rot=pi/2,
						main_con = T, side_con = list(lwd=1, col="red", lty=1) )
	print(x_orig)
	
	x_orig = x_orig+1.5
}
barplot(runif(5), xlim=c(0,7.5), axes=F, width=1.3, space=0.1)
}
