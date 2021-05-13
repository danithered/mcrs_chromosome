tr <- read.table("file:///home/danielred/data/programs/mcrs_chromosome/testgen6.txt", sep="\t")
colnames(tr) <- c("orig", paste0("to", 0: (ncol(tr)-2) ))
tr <- tr[,1:(ncol(tr)-1) ]
str(tr)

# origs <- unique(tr$orig)

# o = origs[2]

# r <- t(sapply(origs,
# function (o) apply(tr[tr$orig == o, ], 2, function(x) mean(x[x != 0]))
# ))
# 
# r

no_types <- ncol(tr)-1
dm <- matrix(NA, ncol=no_types, nrow=no_types)

# for(orr in r[, "orig"] ){
#   for(t in 0:6){
#     #dm[orr,t+1] <- mean(r[r[,"orig"] == orr, paste0("to", t)], r[r[,"orig"] == orr, paste0("to", t)])
#     
#   }
# }

# data.frame(
#   q=log2(tr$orig)[1:28*500-1],
#   w=log2(tr$orig)[1:28*500],
#   r=log2(tr$orig)[1:28*500-2],
#   t=log2(tr$orig)[1:28*500-3],
#   e=c(10:17,1,20,23,24,25,NA,29,NA,NA,NA, NA,NA,NA, 3, 4,5,7,9, NA, NA )
# )

tr[tr == 0] <- NA
tr <- tr[ !is.na(tr$orig),]
tr <- tr[ tr$orig >= 0,]
any(tr$orig %% 2 != 0) # jo esetben F
tr$orig <- log2(tr$orig)

#sum(tr$orig < 0)
#which(is.na(tr$orig))

bplotdist <- function(tr, s1, s2) barplot(table( c(tr[tr$orig==s1, paste0("to", s2)],tr[tr$orig==s2, paste0("to",s1)]) ), main=paste("Distance between", s1, "and", s2))

bplotdist(tr, 1,2)
bplotdist(tr, 1,4)
bplotdist(tr, 2,4)

tr[1,]

for(egyik in 0:(no_types-1) ){
  for(masik in 0:(no_types-1) ){
    dm[egyik, masik] <- median(c(tr[tr$orig == egyik, paste0("to", masik)], tr[tr$orig == masik, paste0("to", egyik)]), na.rm=T)
  }
}
dm <- dm[1:5, 1:5]
library(ape)
#emit 0!!!
# median?


ddm <- as.dist(dm)
ddm[is.na(ddm)] <- 51
rdm <- pcoa(ddm)
gstr(mite.D)
str(ddm)

biplot(rdm, rn=c("E1", "E2", "E12", "E3", "E13"))
biplot(rdm )
length(unique(round(tr$orig)))
