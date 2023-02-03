#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include"gol.h"

void read_in_file(FILE *infile, struct universe *u)
{
	//Variables initialised
	u->rows = 0;
	int count = 0;
	u->columns = 0;
	u->total_alive = 0;
	int x = 0;
	int tempCount;
	int st;
	int start_size_row = 10;
	int start_size_col = 512;
	
	//Dynamically allocates memory to the array grid
	u->grid = malloc(sizeof(char *)* (start_size_row));
	if(u->grid)
	{
		//Allocates memory for the columns
		for ( int i = 0; i < start_size_row; i++ )
		{
    			u->grid[i] = malloc( sizeof(char *) * start_size_col );
		}
	}
	else
	{
		//Error message gets printed if error occurs when allocating memory
		fprintf(stderr,"Couldnt allocate memory for initial array\n");
		exit(1);
	}
	
	//While end of file hasn't been reached it will keep reading each character
	while(EOF!=(st=fgetc(infile)))
	{	
		//Checks to see if there are invalid characters or not in the file/input
		if(st == '*' || st == '.' || st == '\n')
		{
			//Error check to see if number of columns is greater than 512
			if(count > 512)
			{
				fprintf(stderr, "Number of columns is larger than 512\n");
				exit(1);
			}
			if(st == '*')
			{
				u->total_alive++;
				u->grid[u->rows][count]=st;
				count++;
			} 		
			else if(st == '\n')
	  		{
				u->rows++;
				//If the row size is the same as start_size_row then allocates more memory for the rows.
				if(u->rows == start_size_row)
				{
					//Temp variable used to make sure pointer to main array is not lost when realloc fails
					char **store = realloc(u->grid, sizeof(char *)*(start_size_row+=32));
					if(store)
					{
						u->grid = store;
						//Allocates space for the columns for the new rows
						for(int i = 0; i < 32;i++)
						{
			    				u->grid[u->rows + i] = malloc(sizeof(char *)*(start_size_col));
						}
					}
					else
					{
						fprintf(stderr,"Couldnt allocate memory for extra rows!\n");
						exit(1);
					}

				}
				//Instructions only run when first new line is found.
				if(x == 0)
				{
					u->columns = count;
					tempCount = count;
					count = 0;
					x = 1;
				}
				else
				{
					if(tempCount != count)
					{
						fprintf(stderr,"Unequal number of columns.\n");
						exit(1);
					}
					else
					{
						u->columns = count;
						count = 0;
					}
				}
				
	  		}
			else
			{
				//Adds value to array
				u->grid[u->rows][count]=st;
				count++;
			}
		}
		else
		{
			fprintf(stderr, "Invalid characters in input/file.\n");
			exit(1);
		}
	}

	
	//Checks to see if file or stdin is empty
	if(u->columns == 0 || u->rows == 0)
	{
		fprintf(stderr, "Empty input/file\n");
		exit(1);
	}

	//Frees all the extra memory that was allocated for the rows
	for(int i = 0; i <= start_size_row-u->rows;i++)
	{
		free(u->grid[start_size_row-i]);
	}
	u->grid = realloc(u->grid, sizeof(char *) * u->rows);

	//Frees all the extra memory that was allocated for the columns
	for(int i = 0; i < u->rows;i++)
	{
		//Uses new pointer in case realloc fails and we lose original array pointer
		char *new_s =  realloc(u->grid[i], sizeof(char *)*(u->columns));
		if(new_s)
		{
			u->grid[i] = new_s;
		}
		else
		{
			fprintf(stderr, "Error clearing extra columns\n");
			exit(1);
		}
	}

}

void write_out_file(FILE *outfile, struct universe *u)
{
	//Goes through each element in the array and outputs it to the outfile
	for(int i = 0;i<u->rows;i++)
	{
		for(int j = 0; j<u->columns;j++)
		{
			//Checks to see if element could be written to outfile
			if((fprintf(outfile,"%c",u->grid[i][j])) < 0)
			{
				fprintf(stderr, "Problem writing to file!\n");
				exit(1);
			}
		}
		//New line added when row ends
		fprintf(outfile,"\n");
	}

	//Frees the entire grid at the end.
	for(int i = 0;i < u->rows; i++)
	{
		free(u->grid[i]);
	}
	free(u->grid);
	
}

//Get positive modular number.
int wrap_number(struct universe *u, int cell, int size)
{
	int tp;
	
	//If input is row
	if(size == 0)
	{
		//Used to find a positive modular number.
		tp = (cell % u->rows + u->rows) % u->rows;
	}
	//else input must be column
	else
	{
		tp = (cell % u->columns + u->columns) % u->columns;
	}
	return tp;
}

int is_alive(struct universe *u, int column, int row)
{
	//Outside of grid is permanently dead, this will be skipped for torus as it gives positive number
	if(column < 0 || row < 0 || column > u->columns-1 || row > u->rows-1)
	{
		return 0;
	}
	else
	{
		if(u->grid[row][column] == '*')
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

int will_be_alive(struct universe *u, int column, int row)
{
	int alive_count = 0;

	
	//Error checking
	if(row<0 || row > u->rows-1)
	{
		fprintf(stderr, "Invalid row number used!\n");
		exit(1);
	}
	if(column<0 || column > u->columns-1)
	{
		fprintf(stderr, "Invalid column number used!\n");
		exit(1);
	}
	
	//Bottom Right
	if(is_alive(u,column+1,row+1) == 1)
	{
		alive_count++;
	}
	//Right
	if(is_alive(u,column+1,row) == 1)
	{
		alive_count++;
	}
	//Bottom
	if(is_alive(u,column,row+1) == 1)
	{
		alive_count++;
	}
	//Top Right
	if(is_alive(u,column+1,row-1) == 1)
	{
		alive_count++;
	}
	//Bottom Left
	if(is_alive(u,column-1,row+1) == 1)
	{
		alive_count++;
	}
	//Top Left
	if(is_alive(u,column-1,row-1) == 1)
	{
		alive_count++;
	}
	//Left
	if(is_alive(u,column-1,row) == 1)
	{
		alive_count++;
	}
	//Top
	if(is_alive(u,column,row-1) == 1)
	{
		alive_count++;
	}
	
	//Conditions for whether cell will be alive or not next generation
	if(is_alive(u,column,row) == 1 && (alive_count == 2 || alive_count == 3))
	{
		return 1;
	}
	else if(is_alive(u,column,row) == 0 && alive_count == 3)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

int will_be_alive_torus(struct universe *u,  int column, int row)
{
	int alive_cnt = 0;
	//Error checking
	if(row<0 || row > u->rows-1)
	{
		fprintf(stderr, "Invalid row number used!\n");
		exit(1);
	}
	if(column<0 || column > u->columns-1)
	{
		fprintf(stderr, "Invalid column number used!\n");
		exit(1);
	}
	
	//Bottom Right
	if(is_alive(u,wrap_number(u,column+1, 1),wrap_number(u, row+1, 0) ) == 1) 
	{
		alive_cnt++;
	}
	//Right
	if(is_alive(u,wrap_number(u,column+1, 1),wrap_number(u, row, 0) ) == 1)
	{
		alive_cnt++;
	}
	//Bottom
	if(is_alive(u,wrap_number(u,column, 1),wrap_number(u, row+1, 0) ) == 1)
	{
		alive_cnt++;
	}
	//Top Right
	if(is_alive(u,wrap_number(u,column+1, 1),wrap_number(u, row-1, 0) ) == 1)
	{
		alive_cnt++;
	}
	//Bottom Left
	if(is_alive(u,wrap_number(u,column-1, 1),wrap_number(u, row+1, 0) ) == 1)
	{
		alive_cnt++;
	}
	//Top Left
	if(is_alive(u,wrap_number(u,column-1, 1),wrap_number(u, row-1, 0) ) == 1)
	{
		alive_cnt++;
	}
	//Left
	if(is_alive(u,wrap_number(u,column-1, 1),wrap_number(u, row, 0) ) == 1)
	{
		alive_cnt++;
	}
	//Top
	if(is_alive(u,wrap_number(u,column, 1),wrap_number(u, row-1, 0) ) == 1)
	{
		alive_cnt++;
	}

	//Determines if its alive or dead for next generation
	if(is_alive(u,column,row) == 1 && (alive_cnt == 2 || alive_cnt == 3))
	{
		return 1;
	}
	else if(is_alive(u,column,row) == 0 && alive_cnt == 3)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row))
{
	//temp array to store new generation
	char **next;
	int count = 0;
	//Memory allocation for temp array
	next = malloc(sizeof(char *) * (u->rows));
	for(int w = 0; w<u->rows; w++)
	{
		next[w] = malloc(u->columns);
	}
	
	//Runs through each element in array and checks to see if they would be alive or not for next generation
	for(int i = 0;i<u->rows;i++)
	{
		for(int j = 0; j<u->columns;j++)
		{
			if((*rule)(u,j,i) == 1)
			{
				next[i][j] = '*';
				u->total_alive++;
				count++;
			}
			else
			{
				next[i][j] = '.';
			}
		}
	}

	u->final_alive = count;
	//Copies the next array into the grid array. so grid holds latest generation
	memcpy(&u->grid, &next, sizeof(next));
		
}

void print_statistics(struct universe *u)
{
	float i;
	float j;

	//Calculations to find statistics
	if(u->gen == 0)
	{
		i = ((u->total_alive)/((u->rows * u->columns * (u->gen + 1)))) * 100;
		j = i;
	}
	else
	{
		j = (u->final_alive)/(u->rows * u->columns) * 100;
		i = ((u->total_alive)/((u->rows * u->columns * (u->gen + 1)))) * 100;
	}

	printf("%.3f%% of cells currently alive\n", j);
	printf("%.3f%% of cells alive on average\n", i);
}
