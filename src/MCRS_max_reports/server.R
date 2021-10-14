library(shiny)

#functions
update_report <- function(dir, input){
    try(rmarkdown::render("/home/danielred/data/programs/mcrs_chromosome/src/output_graph.Rmd", 
                      params = list(
                           report=input$report,
                           abrmax=input$limits[2],
                           abrmin=input$limits[1],
                           kompl=input$kompl,
                           cache.path=paste0(getwd(), "/MCRS_max_reports/outputs/", input$report, "/cache/")
                      ),
                      output_dir = paste("outputs", input$report, sep="/"),
                      #knit_root_dir = paste("outputs", input$report, sep="/"),
                      intermediates_dir = paste("outputs", input$report, sep="/"),
                      output_file = "index.html"
    ))
    #cat(paste0(getwd(), "/outputs/", input$report, "/cache/\n"))
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

get_remote_dirs <- function(address, ...) get_remote(address=address, what = "dirs", ...)

get_remote_files <- function(address, pattern=NA, ...) {
    out <- get_remote(address=address, what = "files", ...)
    if(is.na(pattern)) {
        return(out)
    }
    return( grep(pattern, out, value = T) )
}

#set default vals
online <- NULL

rootdir="/home/danielred/data/programs/mcrs_chromosome/OUT/"

remote_dirs <- data.frame(name="eti",
                          address = "danielred@148.6.202.1:22023", 
                          key="~/.ssh/id_rsa", 
                          dir="~/data/programs/mcrs_chromosome/OUT")

if(!dir.exists("outputs")) dir.create("outputs")

#server function
shinyServer(function(input, output) {
    if(file.exists("last_updated.rds")){
        upd <- readRDS("last_updated.rds")
    } else {
        upd <- data.frame(id= character(), 
                          output_hash= character(), 
                          abrmax=numeric(),
                          abrmin=numeric(),
                          kompl=logical()
                          )
    }

    output$kompl <- renderUI({
        checkboxInput("kompl",
                      label= "Compute complemeters (lots of time)",
                      value= ifelse( input$report %in% upd$id, 
                                     as.logical(upd[upd$id == input$report, "kompl"]),
                                     FALSE
                             )
        )
    })
    
    output$scale <- renderUI({
        times <- unlist(strsplit(list.files(paste0(rootdir, "/", input$report, "/SAVE"), "*.tsv"), ".", fixed=T))
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
    
    observeEvent(input$knit, {
        times <- unlist(strsplit(list.files(paste0(rootdir, "/", input$report, "/SAVE"), "*.tsv"), ".", fixed=T))
        #times <- times[seq(1, length(times), 2)]
        #times <- times[order(as.numeric(times))]
        
        
        #check if setting or data has been changed
        if(!input$report %in% upd$id) { # it has never been created before 
            #cat("it has never been created before\n")
            
            dir.create(paste0("outputs/", input$report)) # it doesnot even have a dir
            update_report(rootdir, input) # update report

            #create new record at upd
            upd <- rbind(upd, data.frame(id= as.character(input$report),
                                         output_hash= as.character( tools::md5sum(paste0(rootdir, input$report, "/output.csv")) ),
                                         #output_hash= as.character( "hfghgh" ),
                                         abrmax=as.numeric(input$limits[2]),
                                         abrmin=as.numeric(input$limits[1]),
                                         kompl=as.logical(input$kompl)
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
                    update_report(rootdir, input) # update report

                    # update upd
                    upd[ upd$id == input$report, "output_hash"] = tools::md5sum(paste0(rootdir, input$report, "/output.csv"))
                    upd[ upd$id == input$report, "abrmax"] = input$limits[2]
                    upd[ upd$id == input$report, "abrmin"] = input$limits[1]
                    upd[ upd$id == input$report, "kompl"] = input$kompl

                    saveRDS(upd, "last_updated.rds") # save upd
            } # if it has been changed
        }
        
        # serve it
        cat( paste(getwd(), "updated\n") )
        
        if(is.null(online)){
            browseURL(paste(getwd(), "outputs", input$report, "index.html", sep="/"))
        } else {
        
            #addResourcePath("tmpuser", getwd())
            output$rep <- renderUI({
                "starting"
                tags$iframe(#seamless="seamless", 
                            #src= paste("tmpuser/outputs", input$report, "index.html", sep="/"),
                            src= paste(online, input$report, "index.html", sep="/"),
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
