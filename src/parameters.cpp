#include "parameters.h" 

using namespace std;


//parameters harder to change
int par_noEA=3;

//parameters to change with Args()
int par_maxtime = 1;
int par_ncol = 5;
int par_nrow = 5;
int par_output_interval = 0;

char par_ID[255] = "test\0";
char par_str_pool[255] = "IN/mapping.txt";

double par_init_grid = 0.5;
double par_death = 0.5;
double par_diffusion_rate = 0;
double par_ll = 1.1; // l + 1 in equation Rs

//output parameters to file
int paramsToFile(char* filename){
	int i=0;
	
	// open a file in write mode.
	std::fstream paramfile(filename, std::fstream::out);
	
//	std::cout << "Printing parameters to file: " << filename << std::endl;	
	
	paramfile << "MAXLEN " << MAXLEN << std::endl;
	paramfile << "par_maxtime " << par_maxtime << std::endl;
	paramfile << "par_ncol " << par_ncol << std::endl;
	paramfile << "par_nrow " << par_nrow << std::endl;
	paramfile << "par_output_interval " << par_output_interval << std::endl;
	paramfile << "par_ID " << par_ID << std::endl;
	paramfile << "par_death " << par_death << std::endl;
	paramfile << "par_diffusion_rate " << par_diffusion_rate << std::endl;
	paramfile << "par_init_grid " << par_init_grid << std::endl;
	paramfile << "par_ll" << par_ll << std::endl;
	

	paramfile.close();
	return(0);
}


//parameter modifying function
int Args(int argc, char **argv)
{
  int i, s;
  char option;
  //  char temp[BUFSIZE];
  /* parameters may be overridden here. */
  for (i = 1; i < argc; i++) {
	if(argv[i][0] == '-'){
		option = argv[i][1];
		if( option == '-'){ //long expression
			if(!strcmp(argv[i], "--par_death")) option = 'd';
			else if(!strcmp(argv[i], "--par_diffusion_rate")) option = 'D';
			else if(!strcmp(argv[i], "--par_maxtime")) option = 'T';
			else if(!strcmp(argv[i], "--par_ncol")) option = 'c';
			else if(!strcmp(argv[i], "--par_nrow")) option = 'r';
			else if(!strcmp(argv[i], "--par_output_interval")) option = 'o';
			else if(!strcmp(argv[i], "--par_ID")) option = 'I';
			else if(!strcmp(argv[i], "--par_init_grid")) option = 's';
			else if(!strcmp(argv[i], "--par_ll")) option = 'l';
		}
		switch(option){
			// k - par_death
			case 'd':
				if (++i == argc) return 1;
				par_death = atof(argv[i]);
				if(par_death < 0) {
					std::cerr << "ERROR at reading argoments: option " << option << ": par_death cant be negative!" << std::endl;
					return(-1);
				}
				continue;

			case 'l':
				if (++i == argc) return 1;
				par_ll = atof(argv[i]);
				if(par_ll < 0) {
					std::cerr << "ERROR at reading argoments: option " << option << ": par_ll cant be negative!" << std::endl;
					return(-1);
				}
				continue;
			
			case 'D':
				if (++i == argc) return 1;
				par_diffusion_rate = atof(argv[i]);
				if(par_diffusion_rate < 0) {
					std::cerr << "ERROR at reading argoments: option " << option << ": par_diffusion_rate cant be negative!" << std::endl;
					return(-1);
				}
				continue;
			
			case 'T':
				if (++i == argc) return 1;
				par_maxtime = atoi(argv[i]);
				continue;
			
			case 'c':
				if (++i == argc) return 1;
				par_ncol = atoi(argv[i]);
				if(par_ncol <= 0) {
					std::cerr << "ERROR at reading argoments: option " << option << ": par_ncol cant be negative!" << std::endl;
					return(-1);
				}
				continue;
			
			case 'r':
				if (++i == argc) return 1;
				par_nrow = atoi(argv[i]);
				if(par_nrow <= 0) {
					std::cerr << "ERROR at reading argoments: option " << option << ": par_nrow cant be negative!" << std::endl;
					return(-1);
				}
				continue;
			
			case 'o':
				if (++i == argc) return 1;
				par_output_interval = atoi(argv[i]);
				if(par_output_interval < 0) {
					std::cerr << "ERROR at reading argoments: option " << option << ": cant be negative!" << std::endl;
					return(-1);
				}
				continue;
			
			case 's':
				if (++i == argc) return 1;
				par_output_interval = atof(argv[i]);
				if(par_init_grid < 0 || par_init_grid > 1) {
					std::cerr << "ERROR at reading argoments: option " << option << ": have to be between 0 and 1!" << std::endl;
					return(-1);
				}
				continue;
			
			case 'I':
				if (++i == argc) return 1;
				if ( strlen(argv[i]) > 1 ) 
					strcpy(par_ID, argv[i]);
				else {
					std::cerr << "ERROR at reading argoments: option " << option << ": ID should be more than 1 char long!" << std::endl;
					return -1;
				}
				continue;
			
				
			default:
				std::cerr << "ERROR at reading argoments: not valid argoment!" << std::endl;
				return -1;
		}
	}
  }
  return 0;
}
