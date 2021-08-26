#pro month
800000
800000/1.47

#pro tag
800000/20
800000/20/1.47

#montly (3x8)
4*3*800000/20
4*3*800000/20/1.47

#montly (2x8)
4*2*800000/20
4*2*800000/20/1.47

#pro hour
800000/20/8
800000/20/8/1.47

curve(0.5+1/(1+exp(-x*0.5)), xlim=c(0, 10))

0.5+1/(1+exp(-0))

findMinDist <- function(str, from=0, to=length(seq)){
	
	if(is.character(str)) seq = unlist(strsplit(str, ""))
	else seq=str
	
	# if(from > to) {
	# 	tempval = from
	# 	from = to
	# 	to = tempval
	# 	rm(tempval)
	# }
	cat("findMinDist:", seq, from, to, "\n")
	
	if(from <0 | from == to | length(seq) < to) return(list(min_dist=0, n_stems=0))
	
	n_stems = right = left = dist = 0 
	for(letter in seq[from:to]){
		if(letter == '.' ) { if(left==0) dist = dist +1 }
		else if(letter == "(") left = left + 1
		else if(letter == ")"){
			right = right+1
			if(left > 0 & right >= left){
				n_stems = n_stems + 1
				right=left=0
				dist= dist+2
			} 
		} else {
			cat("ERROR: Wrong character is str: '", letter, "'\n", sep="")
			return(NA)
		}
	}
	return(list(min_dist=dist, n_stems=n_stems))
	
}

findMinDist("..((((...)))).",6, 10)

nextstep <-function(pos, dist, slope){
	cat("nextstep: [", pos[1], ", ", pos[2], "] d=", dist, ", slope=", slope, " -> [", pos[1] + cospi(slope) * dist, ", ", pos[2] + sinpi(slope) * dist, "]\n", sep="")
	return( c(x= pos[1] + cospi(slope) * dist, y= pos[2] + sinpi(slope) * dist) )
}
	
dirto <- function(to, from=c(0,0)){
	#x = to[1] - from[1]
	#y = to[2] - from[2]
	#dist = sqrt(x^2 + y^2)
	#return(atan((to[2] - from[2])/(to[1] - from[1])) / pi)
	return(atan2(to[2] - from[2], to[1] - from[1]) / pi)
}


rad2deg <- function(rad) {rad * 180 / pi}
pirad2deg <- function(rad) {rad * 180}
deg2rad <- function(deg) {deg * pi / 180}
deg2pirad <- function(deg) {deg / 180}


plotstrtempl <- function(str, subr){
	str = unlist(strsplit(str, ""))
	length = length(str)
	left = right = 0
	x = y = pairing <- rep(NA, length)
	dangling <- rep(F, length)
	for(a in 1:length) {
		if(str[a] == "(") left = left+1
		else if (str[a] == ")") {
			right = right+1
			pair=a-1
			while(!( str[pair]=="(" & is.na(pairing[pair]) ) | pair < 1 ) pair = pair-1
			pairing[a] = pair
			pairing[pair] = a
		} else if(left==0 | right==left) dangling[a] = T
	}
	
	dist=1
	pos=c(0,-dist)
	direct = 0.5 # in pi rad
	
	a=1
	while(a <= length){
		if(!is.na(x[a])) {
			cat("I will skip this point with", direct, "direction\n")
			pos = c(x=x[a], y=y[a])
			a = a+1
			next
		}
			
		# free
		if( is.na(pairing[a]) ) { 
			if(dangling[a] == F){ # i am a loop
				#compute length of loop
				ll = 1
				while( is.na(pairing[a+ll]) ) ll = ll+1
				if (str[a-1] =="(" & str[a+ll] == ")"){ # it is a closed loop
					rot = ll / (ll+2)
					cat("closed loop detected\n")
				} else { # it is either an interloop or a linker (ignored)
					n_min = ll + 4 + findMinDist(str, pairing[a+ll], pairing[a-1])$min_dist
					#rot = 1 - 1/(ll+2) # simple interloop
					rot = (n_min - 2) / n_min
					cat("interloop detected. n_min", n_min, "ll", ll, "\n")
				}
				
				#direct = direct + 0.5
				direct = dirto(from = c(x[pairing[a-1]], y[pairing[a-1]]), to= c(x[a-1], y[a-1]) )
				cat("ll: ", ll, ", rot: ", rot, ", first direct: ", direct, "\n")
				
				#iterate tru all member
				for(i in 1:(ll+1)){
					direct = direct + rot -1
					pos = nextstep(pos, dist, direct)
					x[a] = pos[1]
					y[a] = pos[2]
					
					a = a+1
				}
				direct = direct + rot -1 + 0.5
				
			} else{ # i am just a free n
				pos = nextstep(pos, dist, direct)
				x[a] = pos[1]
				y[a] = pos[2]
				a = a+1
			}
			
		} else { #has pair
			cat("i have a pair\n")
			
			pos = nextstep(pos, dist, direct)
			x[a] = pos[1]
			y[a] = pos[2]
			#compute neighbour position
			posneigh =  nextstep(pos, dist, direct - 0.5)
			x[pairing[a]] = posneigh[1]
			y[pairing[a]] = posneigh[2]
			a = a+1
		}
			
	}
	
	plot(x,y, type="b", pch=as.character(1:length(x)), asp=1)
	for(m in 1:length) if( !is.na(pairing[m]) ) segments(x[m],y[m], x[pairing[m]], y[pairing[m]], col="red")
	return(list(x=x,y=y,left=left, right=right, pairing=pairing, dangling=dangling))
	
}
plotstrtempl("((.(((...)))))", 1)
plotstrtempl("(((((...))).))", 1)
plotstrtempl("((.(((...))).))", 1)

y=x=rep(NA, 10)
direct=0.5
rot=0.6
pos=c(0,0)
a=1
for(i in 1:10 ){
	direct = direct + rot -1
	pos = nextstep(pos, dist, direct)
	x[a] = pos[1]
	y[a] = pos[2]
	
	a = a+1
}
plot(x, y, asp=1, type="b")
