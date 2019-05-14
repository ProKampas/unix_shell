#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>


int main(int argc , char *argv[])
{
	if(argc>2)
	{
		printf("Error !  Too many arguments!\n");
		printf("The program will now exit\n\n");
		exit(1);
	}
	
	/*--------------------Batch Mode--------------------*/
	if(argc == 2)
	{
		printf("BATCH MODE \n\n\n");
		/*Determine if the batchfile exists and if it is a .txt file
		  If it has an extension but not .txt the program will terminate
		  ,if it does not have an extension at all, an .txt extension will be created*/
		char batch_name[50];	//max pathname 50 chars
		strcpy(batch_name,argv[1]);	
		
		// -------------(1) Check if file exists-------------
		if( access(batch_name,F_OK) != -1 )
		{
			printf("Batch file exists!!!\n\n");
		}
		
		else
		{
			printf("Batch file doesn't exist.\n");
			printf("The program will now terminate\n\n");
			exit(1);
		}
		
		// -------------(2) Check if file has .txt extension-------------
		char *check;
		check = strstr(batch_name,".txt");
		if(check == NULL)
		{
			int i;
			char batch_name1[50];
			strcpy(batch_name1,batch_name);
			printf("The file doesn't have the .txt extension\n");
			printf("The .txt extension will be added to the batch_name\n\n");
			strcat(batch_name1,".txt");
			i = rename(batch_name,batch_name1);
			if(i==0)
				printf("Renaming has been successful\n");
			else
				printf("Renaming has not been successful\n");
			strcpy(batch_name,batch_name1);
		}
		
		else
		{
			printf("The batchfile has the proper .txt extension\n\n");
		}	
		
		
		// -------------(3) Check if the file is empty-------------
		FILE *fp ;
		fp = fopen(batch_name,"r+");
		if( fp == NULL )
		{
			perror("Error while opening the file\n\n");
			exit(1);
		}
		
		//The file has opened successfully
		int c;
		c = getc(fp);
		if (c == EOF) 
		{
			printf("File is empty\n");
			printf("The program will now terminate\n\n");
			exit(1);
		}
		ungetc(c,fp);	//The character that was took it is now placed back to its location in the file
		rewind(fp);		//Restore of the file pointer at the begining of the file
		
		
		/*--------Find number of lines and check if a line has more than 512 characters---------*/
		char ch ; //size of line is 512, plus the '/0' character
		int counter=0, lines=0;
		char s[2] = ";";
		while(!feof(fp))	//Until we reach the end of file
		{
			ch = fgetc(fp);
			counter++;
			if(ch == '\n')
			{
				lines++;
				if(counter>513)
				{
					printf("A line with more than 512 characters exist\n");
					printf("Exiting the program\n\n");
					exit(1);
				}
				counter=0;
			}
		}
		/*----------DEBUG--------*/
		printf("The total lines in the file are %d\n\n",lines);		//Result: Correct number of lines
		/*----------DEBUG--------*/
		
		
		rewind(fp);	//Restore  pointer of position inside the file at the top
	
		/*--------Find how many tokens exist in every line of the file-----------*/		
		for(int i=0; i<lines ; i++)
		{
			char tokens[170][200], *temp , null_term='\0' , space[2]= " " , nullo[1]="";
			char tokens_command[170][200] , line[513];
			int c=1;
			char *tmp;
			int quit_flag=-1, count_tokens_line = 0;		//Number of tokens in a line and a flag if quit is a command in a token
			fgets(line,sizeof(line),fp);	//Read the line
			c = strlen(line);
			if( c>0   &&  line[c-1] == '\n' )
				line[strlen(line)-1] = '\0';
			if(line[c-1] == 13 )
				line[strlen(line)-1] = '\0';
			c=1;
			temp = strtok(line,s);
			if(temp==NULL)
			{
				printf("line %d is empty!",i);
				continue;
			}

			for(int f=0;f<170;f++)
			{
				for(int g=0;g<200;g++)
				{
					tokens[f][g] = '\0';		//temp+1 since strlen does not calc the '\0' character
					tokens_command[f][g] = '\0';
				}
			}
			strcat(tokens[0],temp);
			/*---------DEBUG--------*/
			//printf("temp variable is %s and the length of the string is %d\n",temp,strlen(temp));
			//printf("token variable is %s and the length of the string is %d\n",tokens[0],strlen(tokens[0]));			// Result: Correct 
			/*---------DEBUG--------*/
			
			check = strstr(tokens[0],"quit");
			if(check !=NULL)
				quit_flag=0;	//quit command exists in token[0]
			
			count_tokens_line++;	//the first token has been created
			while(1)
			{
				temp = strtok(NULL,s);
				if( temp == NULL)	//Find the rest tokens in the line
				{
					//	-------DEBUG-------
					//printf("No other tokens in line %d\n\n",i);
					//	-------DEBUG-------
					break;
				}

				c++;
				strcat(tokens[c-1] , temp ) ;
				
				/*---------DEBUG--------*/
				//printf("temp variable is %s and the length of the string is %d\n",temp,strlen(temp));
				//printf("token variable is %s and the length of the string is %d\n",tokens[c-1],strlen(tokens[c-1]));			// Result: Problem 
				/*---------DEBUG--------*/
				
				//Check if the command quit exists inside a token
				check = strstr(tokens[c-1],"quit");
				if(check != NULL)
					quit_flag=c-1;	//represents the token which has the quit command
				
				count_tokens_line++;	//increment the num of tokens in a line 
			}
			
			/*--------------DEBUG--------------*/
			//printf("\nNumber of tokens in a line is------------>  %d\n",count_tokens_line);	//Result: Correct. The last token will either have more space or a new token is created(\r\n)
			/*--------------DEBUG--------------*/
			
			
		   // The array tokens has all the tokens in a line
		   /* They will be examined to find out which of these are blank and then process the rest  */
			int flag=0, flag1=0, d=-1, temp1, temp2;
			for(int j=0 ; j<count_tokens_line ; j++)
			{
				int l=0;
				flag=0;
				counter=0;
				d++ ;
			
				
				if(quit_flag>=0 && flag1==0)	
				{
					if(quit_flag == j)
					{	
						quit_flag = d ;
						flag1 = 1 ;
					}
				}
				if(strlen(tokens[j])==0)	//For commands like ;;;; between the semi-colons the for loop won't be executed
				{							//So, we have to take care of this problem
					//	-------DEBUG-------
					//printf("A token with 0 size has been found(never executed\n");
					//	-------DEBUG-------
					d--;
					continue;
				}
				
				for(int k=0 ; k<(strlen(tokens[j])) ; k++)	//Check every character in every token
				{
					temp2=k;
					//If the character is a white character find if it's a command or an argument
					if(isspace(tokens[j][k]))
					{
						
			//			if(tokens[j][k] == '\n')
			//				continue;
						counter++;
					}
					
			//		printf("\ncounter = %d\n",counter);
					
					
	/*Initial*/		if(counter==k+1)		//Define if at the beginning there are white characters 
					{
						if(counter == strlen(tokens[j])) 	//|| (strlen(tokens[j]) == 0))		//If the token is empty----> covered!
						{
							//-------Debug-------
		//					printf("I got in line 273! The d decreased\n\n");		//Result: Success
							d--;
		//					printf("d ------------------->   %d\n",d);
							break;		//Stop the iteration and search the nexr token.This one is empty
						}
						continue;
					}	
	
					
						
					if(counter<k+1 && flag==0)	//If we read the command it will be placed in the start of the array
					{
						if(k==counter)
						{
							temp1 = counter;	//temp is the number of white characters.If changed, the command is over
							l=0;				//It is not wrong ,it is redundant
						}
						
						if(temp1==counter)
						{
							strncat( tokens_command[d] , &tokens[j][k] , 1);
							l++ ;
							continue;
						}
						if(counter == temp1+1)
						{
							
							strcat( tokens_command[d] , space ) ;		//The command and the arguments are separated with the SPACE ' '
							flag=1;
							l = strlen(tokens_command[d]) ;			//For the next argument
							counter = k ;
							continue;		//IMPORTANT SINCE THE EXECUTION BELOW MUST START AT THE NEXR ITERATION
						}
					}
				
					/*------Place the arguments of the token in proper places-------*/
					// variable l points to the proper position of the array,due to the l++ above
					//counter = k , which means that the first white character after the command is our reference
					//flag=1 means that we look for arguments now
					if(flag == 1)
					{
						if(counter == k)
						{
							temp1=counter ;		//Check if a white character exists.This command is slightly different from the one in line 206
							continue;
						}
						if(counter<k)
						{
							if(counter == temp1+1)
							{
								strncat( tokens_command[d] , space , 2 ) ;		//The command and the arguments are separated with the SPACE ' '
								flag=1;
								l = strlen(tokens_command[d]) ;		//For the next argument
								counter = k ;
								temp1=counter;
								continue;
							}
							else
							{
								strncat( tokens_command[d] , &tokens[j][k] , 1 ) ;	//l++ before the command is not needed since it has already been increased(line 240)
								l++ ;
							}
						}
					}
				}
			}	
			//	-------DEBUG-------
			//	printf("tokens_command[0] = %s\n",tokens_command[0]);
			//	-------DEBUG-------
			/*---------------Debug--------------------*/
			//printf("I have exited the double loop---line 347!\n");		//Result: Success
			//	-------DEBUG-------
			
			
			//If a quit_flag exists we should execute it last. The variable quit_flag shows the token with the quit order inside
			//We have found the proper tokens which are not empty. There are d of these tokens.
			//For each one, the arguments will be passed correctly to the execvp() function
			//and a fork shall be called to begin the execution 
			int *pids, status;	
			d++;	//because we want the last d to be excuted.In the for loop below,that would not happen unless the d increases.
			//malloc for the total number of pids, which are equal to the number of line of the array tokens_commmand (d)
			if((pids = (int *)malloc(d*sizeof(int))) == NULL)   
			{
				printf("Error using malloc\n");
				exit(1);
			}
			
	//		printf("d = %d\n",d);
			char **args;
			char **args1;
			for(int w=0 ; w<d ; w++)
			{
				
				args = (char **)malloc(170*sizeof(char *));
				for(int f=0 ; f<170 ; f++)
				{
					args[f] = (char *)malloc(200*sizeof(char));
					for(int x=0 ; x<200 ; x++)
						args[f][x] = '\0';
				}
				
				/*--------Debug--------*/
		//		printf("\nInside the first for loop line 314!\n");				
				/*USE strtok to pass the arguments in args*/
				temp = strtok(tokens_command[w],space);
				strcat(args[0] , temp);
				c=1;
				
				//	-------DEBUG-------
				//	printf("\nARGS[0]---------->%s\n",args[0]);
				//	-------DEBUG-------
				while(1)
				{
					temp = strtok(NULL,space);
					if( temp == NULL)	//Find the rest tokens in the line
					{
						//	-------DEBUG-------
						//printf("No other tokens in line\n\n");
						//	-------DEBUG-------
						//*args[c] = (char *)NULL;
						break;
					}
					
					c++;
					
					strcat(args[c-1] , temp ) ;
					//	-------DEBUG-------
					//	printf("ARGS[%d] -------------------> %s length %d\n\n",c-1,args[c-1], strlen(args[c-1]));
					//	-------DEBUG-------
					
					/*---------DEBUG--------*/
			//		printf("temp variable is %s and the length of the string is %d\n",temp,strlen(temp));
			//		printf("token variable is %s and the length of the string is %d\n",tokens[c-1],strlen(tokens[c-1]));			// Result: Problem 
					/*---------DEBUG--------*/
				}
				
			//	-------DEBUG-------
			//	printf("C ====== %d\n\n",c);
			//	-------DEBUG-------
				args1 = (char **)malloc((c)*sizeof(char *));
				
				for(int o=0; o<c ; o++)
				{
					if(o==c-1)
					{
						if(isspace(args[c-1][0]))
						{
							//	-------DEBUG-------
							//	printf("The C is decreased!!!!!!\n\n");
							//	-------DEBUG-------
							c=c-1;
							args1 = (char**)realloc(args1,c*sizeof(char *));
							break;
						}
					}
					args1[o] = (char *)malloc((200)*sizeof(char));
					args1[o][0]='\0';
					strncat(args1[o] , args[o] , strlen(args[o]) ) ;
					
					//	-------DEBUG-------
					//	printf("\n\nARGS ARE %s with LENGTH %d\n\n",args1[o],strlen(args1[o]));
					//	-------DEBUG-------
				}
		//		*args1[c]=NULL;
				sleep(1);


				if(w==quit_flag)
				{			
					continue;	// The quit does not execute until all the rest are executed first
				}
				pids[w] = fork();
				if(pids[w]<0)
				{
					printf("Cannot fork()");
					exit(0);
				}
				if(pids[w]==0 && w!=quit_flag)	//the second control is redundant
				{
					
					printf("child process %d\n",w);
					//	-------DEBUG-------
					//	printf("ARGS1[0]------>>>>>>%s\n",args1[0]);
					//	printf("ARGS1[1]------>>>>>>%s\n",args1[1]);
					//	printf("ARGS1[2]------>>>>>>%s\n",args1[2]);
					//	-------DEBUG-------
					execvp(args1[0],args1);	//*args==args[0], the command
					perror(args1[0]);
					exit(0);
				}
				
				while( wait(&status)>0)
				{
					//	-------DEBUG-------
					//	printf("iam in line 507!!!\n");
					//	-------DEBUG-------
				}

				for(int g=0; g<170 ; g++)
				{
					free(args[g]);
				}
				free(args);

		//		for(int g=0 ; g<c ; g++)
		//		{	
		//			free(args1[g]);
		//		}
		//		free(args1);
				sleep(1);			
	
			}
			if(quit_flag>=0)
			{
				//Execute the quit command now 
				//	-------DEBUG-------
				//	printf("Executing quit command!\n\n");
				//	-------DEBUG-------
				exit(0);
			}	
				//	-------DEBUG-------
				//	printf("LINE 411 right before the loop!!\n");
				//	-------DEBUG-------
			free(pids);
		}
							   
										   
		fclose(fp);
	}

	
	
	
	
	
	
	
	
	/*------------------------------------                ----------------------------------------------*/
	/*------------------------------------INTERACTIVE MODE----------------------------------------------*/
	/*------------------------------------                ----------------------------------------------*/
	if(argc == 1)
	{
	  /*--------------------INTERACTIVE MODE--------------------*/
	
		printf("INTERACTIVE MODE \n\n\n");
		char *check;	
		
		int counter=0;
		char s[2] = ";\n";
		
		/*--------Find how many tokens exist in every line of the file-----------*/		
		for(; ;)
		{
			printf("kampas_8151>  ");
			char tokens[170][200], *temp , null_term='\0' , space[2]= " " , nullo[1]="";
			char tokens_command[170][200] , line[513];
			int c;
			char *tmp;
			int quit_flag=-1, count_tokens_line = 0;		//Number of tokens in a line and a flag if quit is a command in a token
			fflush(stdin);
			fgets(line,sizeof(line),stdin);	//Read the line
			c = strlen(line);
			if( c>0   &&  line[strlen(line)-1] == 13 )
				line[strlen(line)-1] = '\0';
			temp = strtok(line,s);
			if(temp==NULL)
			{
				printf("line is empty!\n\n");
				continue;
			}

			for(int f=0;f<170;f++)
			{
				tokens[f][0] = '\0';		
				tokens_command[f][0] = '\0';
			}
			strcat(tokens[0],temp);
			/*---------DEBUG--------*/
		//	printf("temp variable is %s and the length of the string is %d\n",temp,strlen(temp));
		//	printf("token variable is %s and the length of the string is %d\n",tokens[0],strlen(tokens[0]));			// Result: Correct 
			/*---------DEBUG--------*/
			
			check = strstr(tokens[0],"quit");
			if(check != NULL)
				quit_flag=0;	//quit command exists in token[0]
			
			count_tokens_line++;	//the first token has been encountered
			c=1;
			while(1)
			{
				temp = strtok(NULL,s);
				if( temp == NULL)	//Find the rest tokens in the line
				{
					//	-------DEBUG-------
				//	printf("No other tokens in line\n\n");
					//	-------DEBUG-------
					break;
				}

				c++;
				strcat(tokens[c-1] , temp ) ;
				
				/*---------DEBUG--------*/
			//	printf("temp variable is %s and the length of the string is %d\n",temp,strlen(temp));
			//	printf("token variable is %s and the length of the string is %d\n",tokens[c-1],strlen(tokens[c-1]));			// Result: Problem 
				/*---------DEBUG--------*/
				
				//Check if the command quit exists inside a token
				check = strstr(tokens[c-1],"quit");
				if(check != NULL)
					quit_flag=c-1;	//represents the token which has the quit command in the line
				
				count_tokens_line++;	//increment the num of tokens in a line 
			}
			
			/*--------------DEBUG--------------*/
		//	printf("\nNumber of tokens in a line is------------>  %d\n",count_tokens_line);	//Result: Correct. The last token will either have more space or a new token is created(\r\n)
			/*--------------DEBUG--------------*/
			
			
		   // The array tokens has all the tokens in a line
		   /* They will be examined to find out which of these are blank and then process the rest  */
			int flag=0, flag1=0, d=-1, temp1, temp2;
			for(int j=0 ; j<count_tokens_line ; j++)
			{
				int l=0;
				flag=0;
				counter=0;
				d++ ;
			
				
				if(quit_flag>=0 && flag1==0)	
				{
					if(quit_flag == j)
					{	
						quit_flag = d ;
						flag1 = 1 ;
						printf("QUIT IS %d\n\n",quit_flag);
					}
				}
				if(strlen(tokens[j])==0)	//For commands like ;;;; between the semi-colons the for loop won't be executed
				{							//So, we have to take care of this problem
					//	-------DEBUG-------
					//	printf("A token with 0 size has been found(never executed\n");
					//	-------DEBUG-------
					d--;
					continue;
				}
				
				for(int k=0 ; k<(strlen(tokens[j])) ; k++)	//Check every character in every token
				{
					temp2=k;
					//If the character is a white character find if it's a command or an argument
					if(isspace(tokens[j][k]))
					{
						
						if(tokens[j][k] == 13)
							continue;
						counter++;
					}
					
					//	-------DEBUG-------
					//	printf("\ncounter = %d\n",counter);
					//	-------DEBUG-------
					
	/*Initial*/		if(counter==k+1)		//Define if at the beginning there are white characters 
					{
						if(counter == strlen(tokens[j])) 	//|| (strlen(tokens[j]) == 0))		//If the token is empty----> covered!
						{
							//	-------DEBUG-------
							//	printf("I got in line 273! The d decreased\n\n");		//Result: Success
							//	-------DEBUG-------
							d--;
							//	-------DEBUG-------
							//	printf("d ------------------->   %d\n",d);
							//	-------DEBUG-------
							break;		//Stop the iteration and search the nexr token.This one is empty
						}
						continue;
					}	
	
					
						
					if(counter<k+1 && flag==0)	//If we read the command it will be placed in the start of the array
					{
						if(k==counter)
						{
							temp1 = counter;	//temp is the number of white characters.If changed, the command is over
							l=0;				//It is not wrong ,it is redundant
						}
						
						if(temp1==counter)
						{
							strncat( tokens_command[d] , &tokens[j][k] , 1);
							l++ ;
							continue;
						}
						if(counter == temp1+1)
						{
							
							strcat( tokens_command[d] , space ) ;		//The command and the arguments are separated with the SPACE ' '
							flag=1;
							l = strlen(tokens_command[d]) ;			//For the next argument
							counter = k ;
							continue;		//IMPORTANT SINCE THE EXECUTION BELOW MUST START AT THE NEXR ITERATION
						}
					}
				
					/*------Place the arguments of the token in proper places-------*/
					// variable l points to the proper position of the array,due to the l++ above
					//counter = k , which means that the first white character after the command is our reference
					//flag=1 means that we look for arguments now
					if(flag == 1)
					{
						if(counter == k)
						{
							temp1=counter ;		//Check if a white character exists.This command is slightly different from the one in line 206
							continue;
						}
						if(counter<k)
						{
							if(counter == temp1+1)
							{
								strncat( tokens_command[d] , space , 2 ) ;		//The command and the arguments are separated with the SPACE ' '
								flag=1;
								l = strlen(tokens_command[d]) ;		//For the next argument
								counter = k ;
								temp1=counter;
								continue;
							}
							else
							{
								strncat( tokens_command[d] , &tokens[j][k] , 1 ) ;	//l++ before the command is not needed since it has already been increased(line 240)
								l++ ;
							}
						}
					}
				}
			}	
			//	-------DEBUG-------
			//	printf("tokens_command[0] = %s\n",tokens_command[0]);
			//	-------DEBUG-------
			
			//	-------DEBUG-------
			//	printf("I have exited the double loop---line 347!\n");		//Result: Success
			//	-------DEBUG-------
			
			//If a quit_flag exists we should execute it last. The variable quit_flag shows the token with the quit order inside
			//We have found the proper tokens which are not empty. There are d of these tokens.
			//For each one, the arguments will be passed correctly to the execvp() function
			//and a fork shall be called to begin the execution 
			int *pids, status;	
			d++;	//because we want the last d to be excuted.In the for loop below,that would not happen unless the d increases.
			//malloc for the total number of pids, which are equal to the number of line of the array tokens_commmand (d)
			if((pids = (int *)malloc(d*sizeof(int))) == NULL)   
			{
				printf("Error using malloc\n");
				exit(1);
			}
			
			//	-------DEBUG-------
			//  printf("d = %d\n",d);
			//	-------DEBUG-------
			for(int w=0 ; w<d ; w++)
			{
				char **args;
				args = (char **)malloc(170*sizeof(char *));
				for(int f=0 ; f<170 ; f++)
				{
					args[f] = (char *)malloc(200*sizeof(char));
					for(int x=0 ; x<200 ; x++)
						args[f][x] = '\0';
				}
				int a=0;
				int b=0, count=0;
				/*--------Debug--------*/
		//		printf("\nInside the first for loop line 314!\n");				
				/*USE strtok to pass the arguments in args*/
				temp = strtok(tokens_command[w],space);
				strcat(args[0] , temp);
				c=1;
				while(1)
				{
					temp = strtok(NULL,space);
					if( temp == NULL)	//Find the rest tokens in the line
					{
						//	-------DEBUG-------
						//  printf("No other tokens in line\n\n");
						//	-------DEBUG-------
					//	*args[c] = (char *)NULL;		It does not work with my terminal.
						break;
					}

					c++;
					strcat(args[c-1] , temp ) ;

					/*---------DEBUG--------*/
			//		printf("temp variable is %s and the length of the string is %d\n",temp,strlen(temp));
			//		printf("token variable is %s and the length of the string is %d\n",tokens[c-1],strlen(tokens[c-1]));			// Result: Problem 
					/*---------DEBUG--------*/
				}
				char **args1;
				args1 = (char **)malloc((c)*sizeof(char *));
				for(int o=0; o<c ; o++)
				{
					args1[o] = (char *)malloc((200)*sizeof(char));
					strncpy(args1[o] , args[o] , strlen(args[o])+1 ) ;
					args1[o][strlen(args1[o])] = '\0';
					
					//	-------DEBUG-------
					//  printf("\n\nARGS ARE %s with LENGTH %d\n\n",args1[o],strlen(args1[o]));
					//	-------DEBUG-------
				}

				sleep(1);


				if(w==quit_flag)
				{			
					continue;	// The quit does not execute until all the rest are executed first
				}
				pids[w] = fork();
				if(pids[w]<0)
				{
					printf("Cannot fork()");
					exit(0);
				}
				if(pids[w]==0 && w!=quit_flag)	//the second control is redundant
				{
					printf("child process %d\n",w);
					execvp(args1[0],args1);	//*args==args[0], the command
					perror(args1[0]);
					exit(0);
				}
				printf("\n\n\n");
				while( wait(&status)>0)
				{
					printf("iam in line 507!!!\n");
				}

				for(int g=0; g<170 ; g++)
				{
					free(args[g]);
				}
				free(args);

		// 	  It does not work properly in my terminal. I don't know why that happens.
		//		for(int g=0 ; g<c ; g++)		
		//			free(args1[g]);
		//		free(args1);
				sleep(1);			
	
			}
			if(quit_flag>=0)
			{
				//Execute the quit command now 
				printf("Executing quit command!\n\n");
				exit(0);
			}			   
				//	-------DEBUG-------
				//	printf("LINE 411 right before the loop!!\n");
				//	-------DEBUG-------	
			free(pids);
		
		}	 	//infinite loop until the command quit is executed for termination of the program (or another exit command of course)
	
	}
	return 0;
	
}
