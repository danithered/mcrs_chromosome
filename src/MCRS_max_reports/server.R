library(shiny)
library(ssh)

#### functions ####
update_report <- function(dir, input, ssh=NA, key=NA, outdir){
    targetdir <- character()
    if(is.na(ssh) ){
        targetdir <- paste("outputs", input$report, sep="/")
        try(rmarkdown::render("/home/danielred/data/programs/mcrs_chromosome/src/output_graph.Rmd", 
                  params = list(
                       report=input$report,
                       abrmax=input$limits[2],
                       abrmin=input$limits[1],
                       kompl=input$kompl,
                       cache.path=paste0(getwd(), 
                                         #"/MCRS_max_reports/", 
                                         "/",
                                         targetdir, 
                                         "/cache/")
                  ),
                  output_dir = targetdir,
                  #knit_root_dir = paste("outputs", input$report, sep="/"),
                  intermediates_dir = targetdir,
                  output_file = "index.html"
        ))
        #cat(paste0(getwd(), "/outputs/", input$report, "/cache/\n"))
    } else { #on ssh
        #rd <- remote_dirs[remote_dirs$name == simul_names[simul_names$label == input$report, "remote"]]
        targetdir <- paste("outputs", outdir, input$report, sep="/")
        parameters <- list(
            report=input$report,
            abrmax=input$limits[2],
            abrmin=input$limits[1],
            kompl=input$kompl,
            cache.path=paste0(getwd(), 
                              #"/MCRS_max_reports/",
                              "/",
                              targetdir,
                              "/cache/"),
            ssh=TRUE,
            ssh_address=ssh,
            ssh_dir=dir,
            ssh_key=key
        )
        
        try(rmarkdown::render("/home/danielred/data/programs/mcrs_chromosome/src/output_graph.Rmd", 
              params = parameters,
              output_dir = targetdir,
              #knit_root_dir = paste("outputs", input$report, sep="/"),
              intermediates_dir = targetdir,
              output_file = "index.html"
        )) #render
    }#on ssh
    
    return( paste0(substr(targetdir,9,nchar(targetdir)), "/index.html") )
}


get_hash <- function(file, path="", ssh=NA, key){
    if(is.na(ssh)){
        return(tools::md5sum( paste0(path, file) ))
    }
    # it is online
    try({
        #connect to remote host
        if(class(address) == "ssh_session"){
            con = address
            dest = F
        } else {
            con <- ssh_connect(address, keyfile = key)
            dest = T
        }
        
        #get file
        tdir = tempdir()
        scp_download(con, paste0(path, file) , to = tdir)
        
        #get hash
        hash = tools::md5sum( paste0(tdir, file) )
        
        #close con
        if(dest) ssh_disconnect(con)
        
        return(hash)
    })
    
    return(NA)
}

get_remote <- function(address, 
                       path="data/programs/mcrs_chromosome/OUT", 
                       path2="", 
                       key= "~/.ssh/id_rsa",
                       what="all",
                       fullpath=F    
){
    if(!what %in% c("all", "dirs", "files")){
        return(-2)
    }
    
    try({
        #connect to remote host
        if(class(address) == "ssh_session"){
            con = address
            dest = F
        } else {
            con <- ssh_connect(address, keyfile = key)
            dest = T
        }
        
        
        #get data
        if(!ssh_session_info(con)$connected){
            return(-1)
        }
        if(what == "all") {
            out <- ssh_exec_internal(con, command=paste("cd", paste0(path, path2), "\nls -p"))
        } else if(what=="dirs"){
            out <- ssh_exec_internal(con, command=paste("cd", paste0(path, path2), "\nls -d */"))
        } else {
            out <- ssh_exec_internal(con, command=paste("cd", paste0(path, path2), "\nls -p | grep -v /"))
        }
        
        #disconnect
        if(dest) ssh_disconnect(con)
        
        #return data
        if(fullpath){
            return( paste( 
                paste0(path, path2), 
                ( rawToChar(out$stdout) |> strsplit("\\n") )[[1]], 
                sep="/")
            )
        } else {
            return(( rawToChar(out$stdout) |> strsplit("\\n") )[[1]])
        }
    })
    
    return(-3)
}

get_remote_dirs <- function(address, slash=T, ...) {
    out <- get_remote(address=address, what = "dirs", ...)
    if(slash) return(out)
    return( substr(out,1,nchar(out)-1) )
}

get_remote_files <- function(address, pattern=NA, ...) {
    out <- get_remote(address=address, what = "files", ...)
    if(is.na(pattern)) {
        return(out)
    }
    return( grep(pattern, out, value = T) )
}

#### set default vals ####
online <- NULL

rootdir="/home/danielred/data/programs/mcrs_chromosome/OUT/"

remote_dirs <- data.frame(name="eti",
                          address = "danielred@148.6.202.1:22023", 
                          key="~/.ssh/id_rsa", 
                          dir="~/data/programs/mcrs_chromosome/OUT")

if(!dir.exists("outputs")) dir.create("outputs")






#### server function ####
shinyServer(function(input, output) {
    if(file.exists("last_updated.rds")){
        upd <- readRDS("last_updated.rds")
    } else {
        upd <- data.frame(id= character(), 
                          output_hash= character(), 
                          abrmax=numeric(),
                          abrmin=numeric(),
                          kompl=logical(),
                          remote=character()
                          )
    }

    #get simulation IDs
    simul_names <- data.frame(
        names= list.dirs("/home/danielred/data/programs/mcrs_chromosome/OUT", recursive = F, full.names = F),
        remote=NA
        )
    for(rem in remote_dirs$name){
        simul_names <- rbind(simul_names, data.frame(names=get_remote_dirs(
                        address = remote_dirs[remote_dirs$name == rem, "address"],
                        key = remote_dirs[remote_dirs$name == rem, "key"],
                        path = remote_dirs[remote_dirs$name == rem, "dir"],
                        slash=F
                        ),
                   remote=rem
        ))
    }
    simul_names$label <- ifelse(is.na(simul_names$remote), simul_names$names, paste0("[", simul_names$remote, "] ", simul_names$names))
    
    #UI komlementer button (is it on or off)
    output$kompl <- renderUI({
        checkboxInput("kompl",
                      label= "Compute complemeters (lots of time)",
                      value= ifelse( input$report %in% upd$id, 
                                     as.logical(upd[upd$id == input$report, "kompl"]),
                                     FALSE
                             )
        )
    })
    
    #UI list of outputs 
    output$reports <- renderUI({
        selectizeInput("report",
                    label = "Which report would you like to see?",
                    #selectize = F,
                    options=list(maxOptions=nrow(simul_names)),
                    #size= nrow(simul_names),
                    choices= simul_names$label
        )
    })
    
    #UI time slider
    output$scale <- renderUI({
        #get times
        if( is.na(simul_names[simul_names$label == input$report, "remote"]) ){
            times <- unlist(strsplit(list.files(paste0(rootdir, 
                                                       "/", 
                                                       input$report, 
                                                       "/SAVE"), "*.tsv"), 
                                     ".", 
                                     fixed=T)
                            )
        } else {
            rd <- remote_dirs[remote_dirs$name == simul_names[simul_names$label == input$report, "remote"], ]
            times <- get_remote_files(address= rd$address, 
                             key=rd$key, 
                             path=rd$dir,
                             path2=paste0("/", simul_names[simul_names$label == input$report, "names"], "/SAVE" ),
                             pattern = "*.tsv"
            ) |> strsplit(split=".", fixed=T) |> unlist()
        }
        
        #make output
        if(is.null(times)){
            warning("times is NULL! (There are no snatshots in directory)\n")
            h3("No snapshot available")
        } else {
            times <- times[seq(1, length(times), 2)]
            times <- times[order(as.numeric(times))]
        
            sliderInput(
                "limits",
                label="Set minima and maxima of snapshots",
                value=ifelse( rep(input$report %in% upd$id,2), 
                             as.numeric(upd[upd$id == input$report, c("abrmin", "abrmax")]),
                             as.numeric(times[c(1, length(times))]) 
                             ),
                min= as.numeric(times[1]),
                max= as.numeric(times[length(times)]),
                step=1
            )
        }
    })
    
    #pushing knit button
    observeEvent(input$knit, {
        #times <- unlist(strsplit(list.files(paste0(rootdir, "/", input$report, "/SAVE"), "*.tsv"), ".", fixed=T))
        #times <- times[seq(1, length(times), 2)]
        #times <- times[order(as.numeric(times))]
        
        
        #check if setting or data has been changed
        if(!input$report %in% upd$id) { # it has never been created before 
            #cat("it has never been created before\n")
            
            # it doesnot even have a dir
            if( !is.na(simul_names[simul_names$label == input$report, "remote"]) ){
                #create dirs
                dir.create(paste0("outputs/", 
                                  simul_names[simul_names$label == input$report, "remote"])
                           )
                dir.create(paste0("outputs/", 
                                  simul_names[simul_names$label == input$report, "remote"], 
                                  "/", 
                                  simul_names[simul_names$label == input$report, "names"])
                           )
                
                #create report
                input2 <- reactiveValuesToList(input)
                input2$report <- simul_names[simul_names$label == input2$report, "names"]
                td = update_report(dir=remote_dirs[remote_dirs$name == simul_names[simul_names$label == input$report, "remote"], "dir"],
                                   input=input2,
                                   ssh=remote_dirs[remote_dirs$name == simul_names[simul_names$label == input$report, "remote"], "address"],
                                   key=remote_dirs[remote_dirs$name == simul_names[simul_names$label == input$report, "remote"], "key"],
                                   outdir= simul_names[simul_names$label == input$report, "remote"]
                                   ) #still needs to fixed
            } else { #it is local
                #create dir
                dir.create(paste0("outputs/", input$report)) 
                
                #create report
                td = update_report(rootdir, input)
            }
            
            

            #create new record at upd
            upd <- rbind(upd, data.frame(id= as.character(input$report),
                                         output_hash= as.character( get_hash( path=paste0(rootdir, input$report, "/"),
                                                                              file="output.csv",
                                                                              ssh=remote_dirs[remote_dirs$name == simul_names[simul_names$label == input$report, "remote"], "address"] ,
                                                                              key=remote_dirs[remote_dirs$name == simul_names[simul_names$label == input$report, "remote"], "key"])),
                                         #output_hash= as.character( "hfghgh" ),
                                         abrmax=as.numeric(input$limits[2]),
                                         abrmin=as.numeric(input$limits[1]),
                                         kompl=as.logical(input$kompl),
                                         remote= simul_names[simul_names$label == input$report, "remote"] 
                                )
            )
            saveRDS(upd, "last_updated.rds") # save it
        } else { # it has been created before
            if( # hash differences
                upd[ upd$id == input$report, "output_hash"] != tools::md5sum(paste0(rootdir, input$report, "/output.csv")) |
                # abrmax differs
                upd[ upd$id == input$report, "abrmax"] != input$limits[2] |
                # abrmin differs
                upd[ upd$id == input$report, "abrmin"] != input$limits[1] |
                # kompl differs
                upd[ upd$id == input$report, "kompl"] != input$kompl
               ){
                    if( !is.na(simul_names[simul_names$label == input$report, "remote"]) ){
                        #create report
                        input2 <- isolate(reactiveValuesToList(input))
                        input2$report <- simul_names[simul_names$label == input2$report, "names"]
                        td = update_report(dir=remote_dirs[remote_dirs$name == simul_names[simul_names$label == input$report, "remote"], "dir"],
                                           input=input2,
                                           ssh=remote_dirs[remote_dirs$name == simul_names[simul_names$label == input$report, "remote"], "address"],
                                           key=remote_dirs[remote_dirs$name == simul_names[simul_names$label == input$report, "remote"], "key"],
                                           outdir= simul_names[simul_names$label == input$report, "remote"]
                        )
                    } else { #it is local
                        td = update_report(rootdir, input) # update report
                    }

                    # update upd
                    upd[ upd$id == input$report, "output_hash"] = as.character( get_hash( path=paste0(rootdir, input$report, "/"),
                                                                                          file="output.csv",
                                                                                          ssh=remote_dirs[remote_dirs$name == simul_names[simul_names$label == input$report, "remote"], "address"] ,
                                                                                          key=remote_dirs[remote_dirs$name == simul_names[simul_names$label == input$report, "remote"], "key"]))
                    upd[ upd$id == input$report, "abrmax"] = input$limits[2]
                    upd[ upd$id == input$report, "abrmin"] = input$limits[1]
                    upd[ upd$id == input$report, "kompl"] = input$kompl

                    saveRDS(upd, "last_updated.rds") # save upd
            } # if it has been changed
        }
        
        # serve it
        #cat( paste(getwd(), "updated\n") )
        
        if(is.null(online)){
            browseURL(paste(getwd(), "outputs", td, sep="/"))
        } else {
        
            #addResourcePath("tmpuser", getwd())
            output$rep <- renderUI({
                "starting"
                tags$iframe(#seamless="seamless", 
                            #src= paste("tmpuser/outputs", input$report, "index.html", sep="/"),
                            src= paste(online, td, sep="/"),
                            #width="100%",
                            #, height="100hv"
                            margin= 0, 
                            paddin= 0,
                            width= "100%",
                            height= "100%",
                            border= 0,
                            #background-color="#EBEBEB"
                            #, overflow-y= "scroll"
                            )
            })
        }
        
    }) #observer

    
})
