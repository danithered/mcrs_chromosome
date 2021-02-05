300 300 3 Nmet Nrepl ciklusszam mintavetelgyak cellamintavetelgyak 0.8 10 b g 2.0 kmax 0.1 20.0 1.0 0.5 diff 0.01 4

re <- atlagadatok[duplicated(atlagadatok$apc) & atlagadatok$apc != 0,]
for(i in 1:nrow(re)){
	cat("300 300 3", re[i, "metN"], "1 50001 10 1000 0.8 10", re[i, "b"], re[i, "g"], "2.0", re[i, "kmax"], "0.1 20.0 1.0 0.5", re[i, "diff"], "0.01 4\n", file="/home/danielred/params", append = T)
}
