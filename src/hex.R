tm <- matrix(c(sqrt(3)/2, 0, -sqrt(3)/2, -0.5, 1, -0.5,0 ,0,0), ncol=3, byrow=T)
tm2 <- matrix(c(sqrt(3), sqrt(3)/2, 0, 3/2), ncol=2, byrow=T)
v=c(0,-1,1)
(tm %*% v)
plot(0,0, type="n", xlim=c(-4,4), asp=1)
v=c(2,0,-2) 
points(x=(tm %*% v)[1], y=(tm %*% v)[2])

data<- data.frame(x=rep(-1:1, 3), y=rep(-1:1, each=3))

library(plotrix)
hex.plot <- function(data, col) {
  tm <- matrix(c(sqrt(3)/2, 0, -sqrt(3)/2, -0.5, 1, -0.5,0 ,0,0), ncol=3, byrow=T)
  
  plot(0, 0,
       xlim=c(-4,4), asp=1,
       type="n"
  )
  for( coord in 1: nrow(data)) {
    
    data[coord, c(1,2)] <- (tm %*% c(data[coord, 1], data[coord,2], 0-data[coord, 1]-data[coord, 2]))[c(1,2)]
    hexagon(data[coord,1],data[coord,2],unitcell=1,col=NA,border="black")
  }
  
  
  #points(x=data$x, y=data$y)
}
hex.plot <- function(data, col) {
  tm <- matrix(c(sqrt(3)/2, 0, -sqrt(3)/2, -0.5, 1, -0.5,0 ,0,0), ncol=3, byrow=T)
  
  for( coord in 1: nrow(data)) {
    
    data[coord, c(1,2)] <- (tm %*% c(data[coord, 1], data[coord,2], 0-data[coord, 1]-data[coord, 2]))[c(1,2)]
  }
  
  plot(range(data$x), range(data$y),
       type="n"
  )
  points(x=data$x, y=data$y)
}

hex.plot(data)
polygon(0,0, )
library(hexbin)
grid.hexagons()




plot(0, 0,
     xlim=c(-4,4), asp=1,
     type="n"
)

addh <- function(x,y, te=NA, ...){
  hexagon(ifelse(y %% 2 == 0, x, x+0.5), y, ...)
  text(ifelse(y %% 2 == 0, x+0.5, x+1), y+0.5, te)
}

new.hgrid <- function(ncol, nrow){
  
  plot.new()
  plot.window(xlim=c(1, ncol+0.5), ylim=c(nrow+1, 0), asp=1)
  for(col in 1:ncol){
    for(row in 1:nrow){
      addh(col, row, te=paste(col-1, row-1, sep=","))
    }
  }
}

new.hgrid(7, 7)

new.hgrid2 <- function(ncol, nrow){
  
  plot.new()
  plot.window(xlim=c(0, ncol+0.5), ylim=c(nrow+sqrt(1/3), -sqrt(1/3)), asp=1)
  for(n in 0:(nrow*ncol-1)){
    addh(n %% ncol, n %/% ncol, te= paste(n, paste(get.hcoord(ncol=ncol, n=n), collapse = ",") , sep="\n") )
    #cat(n %/% ncol, n %% ncol, "\n")
  }
}

new.hgrid2(6,6)

draw.hgrid <- function(x, ncol, colors, ...){
  scale = (max(x)-min(x) +1 )/length(colors)
  no=length(x)
  if(no %% ncol != 0) cat ("ERROR wrong number of columns")
  plot.new()
  plot.window(xlim=c(0, ncol+0.5), ylim=c(no %/% ncol +sqrt(1/3), -sqrt(1/3)), asp=1, ...)
  for(n in 0:(no-1) ){
    addh(n %% ncol, n %/% ncol, col=colors[round(x[n+1] %/% scale +1)])
    #cat(x[n], colors[round(x[n] %/% scale +1)], "\n")
    #cat(n %/% scale +1, "\n")
  }
}

draw.hgrid(1:9, ncol=3, colors=heat.colors(25))

get.hcoord <- function (ncol, n) {
  y=n %/% ncol
  x = n - y*ncol - y%/%2
  z = 0-x-y
  return(c(x=x, y=y, z=z))
}

ncol=nrow=5
n=10

get.hcoord(ncol=5, 2)

get.n <- function(ncol, x, y) {
  ny = y*ncol
  nx = x + y%/%2
  return(ny+nx)
}

get.n(5, x=2, y=0)

#paratlan nrow eseten nincs torus a pointy vegen












### SQUARE TORUS

library(lattice)
{
#setup grid
ncol=10
nrow=10
k <- matrix(rep(0, ncol*nrow), ncol=ncol)

#set neigh
pos=0
dy=c(-1, 0, 1, -1, 1, -1, 0, 1)
dx=c(-1,-1,-1, 0, 0, 1, 1, 1)
#dx=-15
#dy=+1
  
#calc
k[pos+1] <- 1
for(q in 1:length(dx) ){
  w <-(( pos%/%ncol + dy[q]) %% nrow)*ncol + (pos%%ncol + dx[q]) %% ncol
  k[w+1] <- 2
}

#draw grid
image(1:ncol, 1:nrow, k, ylim=c(nrow+1,0), asp=1, xlab="x", ylab="y",las=1) 
abline(h=0:ncol+0.5, col="grey")
abline(v=0:nrow+0.5, col="grey")
}

### HEX TORUS
{
#setup
ncol=10
nrow=10
k <- rep(0, ncol*nrow)

#set neigh
pos=25
dx=c(-1, -1, 0, 0, 1, 1)
dy=c(0, 1, -1, 1, -1, 0)
#dy=0
#dx=18

#calc
k[pos+1] <- 1
for(q in 1:length(dx) ){
  #w <- (( pos%/%ncol + dy[q]) %% nrow)*ncol + (pos%%ncol + dx[q] - ((pos%/%ncol) %/% 2)) %% ncol
  #w <- (( pos%/%ncol + dy[q]) %% nrow)*ncol + (pos%%ncol + dx[q] ) %% ncol
  w <- (( pos%/%ncol + dy[q]) %% nrow)*ncol + (pos%%ncol + dx[q] + (dy[q] + bitwAnd(pos %/% ncol, 1))%/%2 ) %% ncol
  #w <- (( pos%/%ncol + dy[q]) %% nrow)*ncol + (pos%%ncol + dx[q] + dy[q] + bitwAnd(pos %/% ncol,1) -1 ) %% ncol
  k[w+1] <- 2
}

#draw grid
draw.hgrid(k, ncol, colors=c("white", "red", "green"))
}











### neigh types square
for(neigh_tipus in 3:30){
  n_inic_x = n_inic_y = c()
  maxDist = ceiling(log2( neigh_tipus - 1))
  for(x  in -maxDist:maxDist){ 
    for(y in -maxDist:maxDist){ 
      if( 2^abs(x) + 2^abs(y) <= neigh_tipus ){
        n_inic_x <- c(n_inic_x, x)
        n_inic_y <- c(n_inic_y, y)
      }
    } 
  }
  
  k <- matrix(rep(0, (maxDist*2+1)^2 ), ncol=maxDist*2+1)
  middle <- maxDist + 1
  
  for(i in 1:length(n_inic_x)){
    k[middle + n_inic_x[i], middle +n_inic_y[i] ] <- 1
  }
  k[middle+0,middle+0] <- 2
  
  #draw grid
  image(-maxDist:maxDist, -maxDist:maxDist, k, 
        ylim=c(-maxDist-0.5,maxDist+0.5),
        xlim=c(-maxDist-0.5,maxDist+0.5),
        asp=1, 
        xlab="", ylab="", axes=F,
        las=1,
        bty="n",
        main=neigh_tipus) 
  segments(-(maxDist+1):maxDist+0.5, 
           rep(-maxDist-0.5, maxDist*4 ), 
           -(maxDist+1):maxDist+0.5, 
           rep(maxDist+0.5, maxDist*4 ),
           col="grey")
  segments( y0=-(maxDist+1):maxDist+0.5, 
            x0=rep(-maxDist-0.5, maxDist*4 ), 
            y1=-(maxDist+1):maxDist+0.5,
            x1=rep(maxDist+0.5, maxDist*4 ),
            col="grey")
  #abline(h=-(maxDist+1):maxDist+0.5, col="grey")
  #abline(v=-(maxDist+1):maxDist+0.5, col="grey")
  axis(1, lwd=0, at=-maxDist:maxDist)
  axis(2, lwd=0, at=-maxDist:maxDist, las=1)
}








### HEX neigh types
for(neigh_tipus in 4:60){
  dx=c()
  dy=c()
  maxDist = round(log2(neigh_tipus - 2))
  for(x in -maxDist:maxDist){ 
    for(y in -maxDist:maxDist){ 
      if( (2^abs(x) + 2^abs(y) + 2^abs(0-x-y)) <= neigh_tipus ) {
        dx <- c(dx, x);
        dy <- c(dy, y);
      }
    }
  } 
  
  
  #setup
  ncol=nrow=maxDist*2+1
  k <- rep(0, ncol*nrow)
  
  #set neigh
  pos=floor(length(k)/2)
  
  #calc
  for(q in 1:length(dx) ){
    w <- (( pos%/%ncol + dy[q]) %% nrow)*ncol + (pos%%ncol + dx[q] + (dy[q] + bitwAnd(pos %/% ncol, 1))%/%2 ) %% ncol
    k[w+1] <- 1
  }
  k[pos+1] <- 2
  
  #draw grid
  draw.hgrid(k, ncol, colors= c("transparent",rev(heat.colors(2))), main=neigh_tipus)
  mtext(neigh_tipus)
}
