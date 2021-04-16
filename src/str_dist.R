tr <- read.table("file:///home/danielred/data/programs/mcrs_chromosome/testgen.txt", sep="\t")[,1:8]
colnames(tr) <- c("orig", paste0("to", 0:6))
str(tr)

# origs <- unique(tr$orig)

# o = origs[2]

# r <- t(sapply(origs,
# function (o) apply(tr[tr$orig == o, ], 2, function(x) mean(x[x != 0]))
# ))
# 
# r

dm <- matrix(NA, ncol=7, nrow=7)

# for(orr in r[, "orig"] ){
#   for(t in 0:6){
#     #dm[orr,t+1] <- mean(r[r[,"orig"] == orr, paste0("to", t)], r[r[,"orig"] == orr, paste0("to", t)])
#     
#   }
# }

tr[tr == 0] <- NA

bplotdist <- function(tr, s1, s2) barplot(table( c(tr[tr$orig==s1, paste0("to", s2)],tr[tr$orig==s2, paste0("to",s1)]) ), main=paste("Distance between", s1, "and", s2))

bplotdist(tr, 1,2)
bplotdist(tr, 1,4)
bplotdist(tr, 2,4)

tr[1,]

for(egyik in 0:6 ){
  for(masik in 0:6){
    dm[egyik, masik] <- median(c(tr[tr$orig == egyik, paste0("to", masik)], tr[tr$orig == masik, paste0("to", egyik)]), na.rm=T)
  }
}
dm <- dm[1:5, 1:5]
library(ape)
#emit 0!!!
# median?


ddm <- as.dist(dm)
ddm[is.na(ddm)] <- 40
rdm <- pcoa(ddm)
gstr(mite.D)
str(ddm)

biplot(rdm, rn=c("E1", "E2", "E12", "E3", "E13"))
