//Union to show dual behavior:

#include <stdio.h>
#include <string.h>
union name
{
   char name_1[50], name_2[50];
};
 
int main()
{
    union name n;
 
    strcpy(n.name_1, "yash");
    printf("\nWe set name_1 to 'yash'. name_1 : %s, name_2 = %s\n", n.name_1, n.name_2);
 
    strcpy(n.name_2, "gugale");
    printf("\nWe set name_2 to 'gugale'. name_1 : %s, name_2 = %s\n", n.name_1, n.name_2);
    
    printf("Hence, we observe that both variables take the same location in memory.\n");
    return 0;
}
