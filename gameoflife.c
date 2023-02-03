#include<stdio.h>
#include"gol.h"
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<errno.h>
#include<limits.h>

int main(int argc, char *argv[])
{
	struct universe v;

	//Default values
	char *inp = "*";
	char *out = "*";
	int i_count = 0;
	int o_count = 0;
	int g_count = 0;
	FILE *fp;
	FILE *op;
	v.gen = 5;
	v.torus = 0;
	v.stat = 0;
	int no_of_arg = 0;

	//Goes through every argument in command line apart from first
	for(int i = 1; i < argc; i++)
	{
		//Only checks arguments that start with '-'
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
				case 'i':
					//Checks to see if previous argument was input or output and then skips this case
					if(strcmp(argv[i-1],"-i") == 0 || strcmp(argv[i-1],"-o") == 0)
					{
						break;
					}
					//Checks to see if there is an argument after this or if next argument starts with '-'
					else if(i+1 == argc)
					{
						fprintf(stderr, "Input file not entered\n");
						exit(1);
						break;
					}
					//Checks to see if same argument has been passed more than once and whether they are the same or not
					else if(i_count > 0)
					{
						if(strcmp(inp,argv[i+1]) != 0)
						{
							fprintf(stderr,"Inputted too many input files\n");
							exit(1);
						}
						else
						{
							no_of_arg = no_of_arg + 2;
						}
					}
					else
					{
						inp = argv[i+1];
						i_count++;
						no_of_arg = no_of_arg + 2;
					}
					break;
				case 'o':
					//Checks to see if previous argument was input or output and then skips this case
					if(strcmp(argv[i-1],"-i") == 0 || strcmp(argv[i-1],"-o") == 0)
					{
						break;
					}
					else if(i+1 == argc)
					{
						fprintf(stderr, "Output file not entered\n");
						exit(1);
					}
					else if(o_count > 0)
					{
						if(strcmp(out,argv[i+1]) != 0)
						{
							fprintf(stderr,"Inputted too many output files\n");
							exit(1);
						}
						else
						{
							no_of_arg = no_of_arg + 2;
						}
					}
					else
					{
						out = argv[i+1];
						o_count++;
						no_of_arg = no_of_arg + 2;
					}
					
					break;
				case 'g':
					//Checks to see if previous argument was input or output and makes sure that the previous of that wasn't an input or output  
					if((strcmp(argv[i-1],"-i") == 0 || strcmp(argv[i-1],"-o") == 0) && (strcmp(argv[i-2],"-i") != 0 || strcmp(argv[i-2],"-o") != 0))
					{
						break;
					}
					//Checks to see if a value for generations has been inputted
					else if(i+1 == argc || argv[i+1][0] == '-')
					{
						fprintf(stderr, "Invalid number of generations\n");
						exit(1);
					}
					else
					{
						//Checks if user input is a digit or not and whether the input entered is out of range for long and int
						char *end;
						errno = 0;
						long int temp = strtol(argv[i+1], &end, 10);
						if(errno == ERANGE)
						{
							fprintf(stderr,"Number of generations not representable by long\n");
							exit(1);
						}
						else if(temp > INT_MAX)
						{
							fprintf(stderr,"Number of generations not representable by int\n");
							exit(1);
						}
						else if(*end == '\0')
						{
							if(temp < 0)
							{
								fprintf(stderr, "Invalid integer\n");
								exit(1);
							}
							else
							{
								//Checks to see if generations have been inputted more than once and if so, are they are same?
								if(g_count > 0)
								{
									if(v.gen != temp)
									{
										fprintf(stderr,"Number of generations inputted differ\n");
										exit(1);
									}
									else
									{
										no_of_arg = no_of_arg + 2;
									}
								}
								else
								{
									v.gen = temp;
									g_count++;
									no_of_arg = no_of_arg + 2;
								}
							}
						}
						else
						{
							fprintf(stderr, "Invalid integer\n");
							exit(1);
						}
					}
					break;
				case 's':
					if((strcmp(argv[i-1],"-i") == 0 || strcmp(argv[i-1],"-o") == 0) && (strcmp(argv[i-2],"-i") != 0 || strcmp(argv[i-2],"-o") != 0))
					{
						break;
					}
					else
					{
						//Sets stat value
						v.stat = 1;
						no_of_arg++;
					}
					break;
				case 't':
					if((strcmp(argv[i-1],"-i") == 0 || strcmp(argv[i-1],"-o") == 0) && (strcmp(argv[i-2],"-i") != 0 || strcmp(argv[i-2],"-o") != 0))
					{
						break;
					}
					else
					{
						//Sets torus value
						v.torus = 1;
						no_of_arg++;
					}
					break;
				default:
					if((strcmp(argv[i-1],"-i") == 0 || strcmp(argv[i-1],"-o") == 0) && (strcmp(argv[i-2],"-i") != 0 || strcmp(argv[i-2],"-o") != 0))
					{
						break;
					}
					else
					{
						fprintf(stderr, "Invalid argument\n");
						exit(1);
					}
					break;
				
			}	
		}
	}
	
	//If there are additional inputs compared to what was expected then error
	if(no_of_arg < argc-1)
	{
		fprintf(stderr, "Invalid argument(s)\n");
		exit(1);
	}
	
	//If user has not supplied a file then take user input
	if(strcmp(inp,"*") == 0)
	{
		read_in_file(stdin,&v);
		
	}
	else
	{
		fp = fopen(inp,"r");
		//If file does not exist then error is printed
		if(fp == NULL)
		{
			fprintf(stderr, "Could not open file!\n");
			exit(1);
		}
		else
		{
			read_in_file(fp,&v);
			fclose(fp);	
		}
	}

	//If torus has been specified then run evolve with torus parameter else use normal
	if(v.torus == 1)
	{
		for(int i = 0; i<v.gen;i++)
		{
			evolve(&v,will_be_alive_torus);
		}

	}
	else
	{
  		for(int i = 0; i<v.gen;i++)
		{
			evolve(&v,will_be_alive);
		}

	}

	//Checks to see if output file has been inputted, else prints on screen
	if(strcmp(out,"*") == 0)
	{
		write_out_file(stdout,&v);
		if(v.stat == 1)
		{
			print_statistics(&v);
		}
	}
	else
	{
		op = fopen(out,"w");
		if(op == NULL)
		{
			fprintf(stderr, "Invalid output file name!\n");
			exit(1);
		}
		else
		{
			write_out_file(op,&v);
			if(v.stat == 1)
			{
				print_statistics(&v);
			}
			fclose(op);
		}
	}

	return 0;
}
