#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int count = 0;
static int time = 0;
char conflict[100]="";
int resource_changed = 0;
char outline[50] ="##################======*======##################";

void print_resouce();

static struct Resource
{
    int resource[10][3];
} resource;

int total_transaction = -1;
int transaction_operations[10] = {0};

static struct Transaction_table
{
    int sl_no[20];
    struct Transaction 
    {
        int id;
        int time;
        struct Data 
        {
            int index;
            int value;
        } transaction_holds;
        char tFun[100];
    } transaction[10][20];
} table;

int inti_resource()
{
    FILE *file = fopen("resource.txt", "r"); 
    if (file == NULL)
    {
        printf("Error: Could not open resource file.\n");
        return -1;
    }

    for (int i = 0; i < 10; i++)
    {
        fscanf(file, "%d %d", &resource.resource[i][0], &resource.resource[i][1]);
        resource.resource[i][2] = -1;
    }

    fclose(file); 

    printf("Resource initialized from file:\n");
    for (int i = 0; i < 10; i++)
    {
        printf("Resource[%d]: Value = %d, Lock Status = %d\n", i, resource.resource[i][0], resource.resource[i][2]);
    }

    return 0;
}

int incremeant_time()
{
    time = time + 1;
    return 0;
}

int create_Transaction()
{
    struct Transaction transaction;
    transaction.id = count;
    transaction.time = time;
    strcpy(transaction.tFun, "init");

    table.sl_no[count] = count;
    table.transaction[count][0] = transaction;
    transaction.transaction_holds.index = -1;
    transaction.transaction_holds.value = -1;
    total_transaction++;
    
    printf("\n\nA transaction created.........: %d\n", transaction.id);
    printf("Transaction details\nTransaction id: %d\nTransaction time: %d\nTransaction value: %d\nTransaction function: %s\n",
           transaction.id, transaction.time, transaction.transaction_holds.value, transaction.tFun);
    
    count++;
    return transaction.id;
}

bool check_Id(int tid)
{
    if(tid > total_transaction || tid < 0)
    {
        printf("%s\n\n\tThe Entered tid is not valid.\n%s\n",outline,outline);
        return true;
    }

    return false;
}

bool check_changes()
{
    if(resource_changed <= 0)
    {
        printf("\n%s\n\t   Nothing to commit\n%s",outline,outline);
        return true;
    }
    return false;
}

int read_operation(int tid, int resource_index)
{

    if(check_Id(tid))
    {
        return -1;
    }
    
    printf("\n...........The read operation...........:\n");
    printf("on T%d is being performed\n", tid);

    int op_count = transaction_operations[tid];  
    struct Transaction new_transaction = table.transaction[tid][op_count];
    
    if (resource.resource[resource_index][2] != -1 && resource.resource[resource_index][2] != tid)
    {
        printf("Resource %d is locked by another transaction (T%d).\n", resource_index, resource.resource[resource_index][2]);
        return -1;
    }

    printf("Do you want to lock this resource? (1 for Yes, 0 for No): ");
    int lock_choice;
    scanf("%d", &lock_choice);

    if (lock_choice == 1)
    {
        resource.resource[resource_index][2] = tid;
        printf("Resource %d locked by T%d.\n", resource_index, tid);
    }

    new_transaction.transaction_holds.index = resource_index;
    new_transaction.transaction_holds.value = resource.resource[resource_index][0];

    resource.resource[resource_index][1]++;
    new_transaction.time = time;

    int r_value = resource.resource[resource_index][0];
    snprintf(new_transaction.tFun, sizeof(new_transaction.tFun), "read of r[%d] = %d",resource_index, r_value);

    table.transaction[tid][op_count + 1] = new_transaction;
    transaction_operations[tid]++;

    printf("The transaction sl_no in the table is %d\n", table.sl_no[tid]);
    printf("Read resource value: %d from resource[%d]\n", new_transaction.transaction_holds.value, resource_index);

    return 0;
}

bool check_resouce(int tid)
{   
    int val = table.transaction[tid][transaction_operations[tid]].transaction_holds.value; 
    printf("Executing here.....%d %d",val);
    if( val <= 0 )
    {
        printf("\n%s\nThe transactions has not locked any resources... yet\nPlease execute read operation first\n%s",outline,outline);
        return true;
    }
    return false;
}

int Artithmatic_operation(int tid)
{
    if(check_Id(tid) || check_resouce(tid))
    {
        return -1;
    }

    printf("\n:...........The Arithmetic operation...........:\n");
    printf("on T%d is being performed\n", tid);

    int op_count = transaction_operations[tid];  
    struct Transaction new_transaction = table.transaction[tid][op_count];
    
    int before_value = new_transaction.transaction_holds.value;
    int add_it = 100;

    int choice = -1;
    printf("press 1 for Add opertion\npress any key to perform subtract operation: ");
    scanf("%d",&choice);
    
    if(choice == 1)
    {
            printf("Enter the value need to be added:");
        scanf("%d",&add_it);

        new_transaction.transaction_holds.value += add_it;
        new_transaction.time = time;

        snprintf(new_transaction.tFun, sizeof(new_transaction.tFun), "Arithmetic operation of %d + %d", before_value, add_it);

        table.transaction[tid][op_count + 1] = new_transaction;
        transaction_operations[tid]++;

        printf("\nThe transaction sl_no in the table is %d\n", table.sl_no[tid]);
        printf("Resource value: %d from resource[%d]\n", new_transaction.transaction_holds.value, new_transaction.transaction_holds.index);
        return 1;
    }

    printf("Enter the value need to be subtracted:");
    scanf("%d",&add_it);

    int min = new_transaction.transaction_holds.value - add_it;

    if(min <= 10)
    {
        printf("\n%s\nThis operation cannot be performed .\n%s\n",outline,outline);
        return -1;
    } 

    new_transaction.transaction_holds.value -= add_it;
    new_transaction.time = time;

    snprintf(new_transaction.tFun, sizeof(new_transaction.tFun), "Arithmetic operation of %d - %d", before_value, add_it);

    table.transaction[tid][op_count + 1] = new_transaction;
    transaction_operations[tid]++;

    printf("The transaction sl_no in the table is %d\n", table.sl_no[tid]);
    printf("Resource value: %d from resource[%d]\n", new_transaction.transaction_holds.value, new_transaction.transaction_holds.index);

    return 2;
}

int write_operation(int tid)
{
    if(check_Id(tid) || check_resouce(tid))
    {
        return -1;
    }

    printf("\n...........The write operation...........:\n");
    printf("on T%d is being performed\n", tid);
    
    print_resouce();

    int op_count = transaction_operations[tid];
    struct Transaction new_transaction = table.transaction[tid][op_count];

    if (resource.resource[new_transaction.transaction_holds.index][2] != tid)
    {
        printf("Resource %d is not locked by T%d. Cannot perform write operation.\n", new_transaction.transaction_holds.index, tid);
        return -1;
    }

    int resource_index = new_transaction.transaction_holds.index; 
    int before_value = resource.resource[resource_index][0];

    resource.resource[resource_index][0] = new_transaction.transaction_holds.value;
    new_transaction.time = time;

    snprintf(new_transaction.tFun, sizeof(new_transaction.tFun), "Writing on resource[%d]: %d -> %d", resource_index, before_value, resource.resource[resource_index][0]);

    table.transaction[tid][op_count + 1] = new_transaction;
    transaction_operations[tid]++;

    printf("The transaction sl_no in the table is %d\n", table.sl_no[tid]);
    printf("Resource value: %d from resource[%d]\n", new_transaction.transaction_holds.value, new_transaction.transaction_holds.index);
    
    resource.resource[resource_index][1]--;
    resource.resource[resource_index][2] = -1;  
    print_resouce();

    resource_changed += 1;

    return 0;
}


int commit_operation(int tid)
{
    
    if(check_Id(tid) || check_changes() || check_resouce(tid))
    {
        return -1;
    }

    printf("\n...........The commit operation...........:\n");
    printf("on T%d is being performed\n", tid);

    print_resouce();

    int op_count = transaction_operations[tid];
    struct Transaction new_transaction = table.transaction[tid][op_count];

    // Commit the transaction
    new_transaction.time = time;
    snprintf(new_transaction.tFun, sizeof(new_transaction.tFun), "commit on t%d", tid);

    table.transaction[tid][op_count + 1] = new_transaction;
    transaction_operations[tid]++;

    FILE *file = fopen("resource.txt", "w");
    if (file == NULL)
    {
        printf("Error: Could not open resource file for writing.\n");
        return -1;
    }

    for (int i = 0; i < 10; i++)
    {
        fprintf(file, "%d %d\n", resource.resource[i][0], resource.resource[i][1]);
    }

    fclose(file); 

    printf("The transaction sl_no in the table is %d\n", table.sl_no[tid]);
    printf("Resource value: %d from resource[%d]\n", new_transaction.transaction_holds.value, new_transaction.transaction_holds.index);

    resource_changed -= 1;

    return 0;
}


void print_resouce()
{   
    printf("\nResource values : \n");
    for(int i = 0; i < 10; i++)
    {
        printf("\t%d ", resource.resource[i][0]);
    }
    printf("\n");
}

void print_table()
{
    printf("\n=============================< The Transaction Table >=============================\n");
    printf("\n\tsl_no\tid\ttime\tR_value\t\tFunction\n");

    for (int i = 0; i < count; i++)
    {
        printf("\n----------------------------------------------------------------------------------\n");
        printf("\t %d", table.sl_no[i]);
        for (int j = 0; j <= transaction_operations[i]; j++)
        {
            if(j!=0)
            {
                printf("\t");
            }
            printf("\t%d \t%d \t%d \t%s \n",
                   table.transaction[i][j].id,
                   table.transaction[i][j].time,
                   table.transaction[i][j].transaction_holds.value,
                   table.transaction[i][j].tFun);
        }
    }
    printf("\n==================================================================================\n");
}
int menu()
{
    int choice;
    while (1)
    {
        printf("\nMenu:\n");
        printf("1. Create Transaction\n");
        printf("2. Read Operation\n");
        printf("3. Arithmetic Operation\n");
        printf("4. Write Operation\n");
        printf("5. Commit Operation\n");
        printf("6. Print Transaction Table\n");
        printf("7. Print Resource Status\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1)  
        {
            printf("Invalid input. Please enter an integer.\n");
            while (getchar() != '\n');  
            continue;
        }

        int tid, resource_index;

        switch (choice)
        {
            case 1:
                create_Transaction();
                break;
            case 2:
                printf("Enter Transaction ID: ");
                if (scanf("%d", &tid) != 1)                  {
                    printf("Invalid input. Please enter an integer.\n");
                    while (getchar() != '\n');
                    continue;
                }
                printf("Enter Resource Index: ");
                if (scanf("%d", &resource_index) != 1)
                {
                    printf("Invalid input. Please enter an integer.\n");
                    while (getchar() != '\n'); 
                    continue;
                }
                read_operation(tid, resource_index);
                break;
            case 3:
                printf("Enter Transaction ID: ");
                if (scanf("%d", &tid) != 1) 
                {
                    printf("Invalid input. Please enter an integer.\n");
                    while (getchar() != '\n');
                    continue;
                }
                Artithmatic_operation(tid);
                break;
            case 4:
                printf("Enter Transaction ID: ");
                if (scanf("%d", &tid) != 1)  
                {
                    printf("Invalid input. Please enter an integer.\n");
                    while (getchar() != '\n');
                    continue;
                }
                write_operation(tid);
                break;
            case 5:
                printf("Enter Transaction ID: ");
                if (scanf("%d", &tid) != 1)
                {
                    printf("Invalid input. Please enter an integer.\n");
                    while (getchar() != '\n');
                    continue;
                }
                commit_operation(tid);
                break;
            case 6:
                print_table();
                break;
            case 7:
                print_resouce();
                break;
            case 8:
                printf("Exiting...\n");
                exit(0);
                break;
            default:
                printf("Invalid choice! Try again.\n");
        }

        incremeant_time();
    }
    return 0;
}

int main()
{
    inti_resource();

    menu();

    return 0;
}   