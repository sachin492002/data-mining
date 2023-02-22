#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define ceil(x, y) (((x) + (y)-1) / (y))

int supp = 0;
char *nullChar = "\0";

typedef struct nodes // node in fp tree
{
    int count;
    int item;
    struct nodes *parent;
    struct nodes *sibling;
    struct nodes *child;
    struct nodes *level;
} node;

typedef struct header
{
    int count;
    char *item;
    node *addr;
    node *prev;
} headerTable;

typedef struct miningTables
{
    int count;
    char item;
    node *addr;
    node *prev;
} miningTable;

typedef struct Orderd
{
    int array[15];
    int len;
    int count;
} Ordered;

// initialization for each node in fp-tree, items_table, etc.

void init_node(node *temp)
{
    temp->count = 0;
    temp->item = -1;
    temp->parent = temp->sibling = temp->child = temp->level = NULL;
}
void init_table(headerTable *temp)
{
    temp->count = 0;
    temp->item = nullChar;
    temp->addr = NULL;
    temp->prev = NULL;
}
void init_miningTable(miningTable *temp)
{
    temp->count = 0;
    temp->item = -1;
    temp->addr = NULL;
    temp->prev = NULL;
}
void init_Ordered(Ordered *temp)
{
    temp->len = 0;
    temp->count = 0;
}
int find_index(int item, miningTable miningCounter[], int miningCount)
{
    int i = -1;
    for (i = 0; i <= miningCount; ++i)
    {
        if (miningCounter[i].item == item)
            break;
    }
    return i;
}

void sortDesc(headerTable counter[10000], int last_index) // sort the headerTable in decending order
{
    int i, j;
    headerTable temp;

    for (i = 0; i < last_index - 1; i++)
    {
        for (j = 0; j < (last_index - 1 - i); j++)
        {
            if (counter[j].count < counter[j + 1].count)
            {
                temp = counter[j];
                counter[j] = counter[j + 1];
                counter[j + 1] = temp;
            }
        }
    }
}

void remove_infrequent(headerTable counter[10000], int *item_count) // remove infrequent itemsets from headerTable which does not have min required support
{
    int last_index = *item_count;
    int sup_count = supp;

    int i = 0, j;
    while (i <= last_index)
    {
        if (counter[i].count < sup_count)
        {
            for (j = i; j < last_index; j++)
                counter[j] = counter[j + 1];
            last_index--;
        }
        else
            i++;
    }
    *item_count = last_index;
}

void Ordering_Transactions(char *transItem, headerTable counter[], Ordered table[], int order_row, int item_count) // order the transaction items according to the sorted headerTable
{
    char *data_item = (char *)malloc(1000 * sizeof(char));
    int k = 0;
    int trans_iter = 0;
    int found;
    init_Ordered(&table[order_row]);

    for (int i = 0; i <= item_count; ++i)
    {
        trans_iter = 0;
        found = 0;
        while (transItem[trans_iter] != '\0' && found == 0)
        {

            if (transItem[trans_iter] != ',')
                data_item[k++] = transItem[trans_iter];
            else
            {
                data_item[k] = '\0';
                if (strcmp(data_item, counter[i].item) == 0)
                {
                    table[order_row].array[table[order_row].len++] = i;
                    found = 1;
                }
                free(data_item);
                data_item = NULL;
                data_item = (char *)malloc(1000 * sizeof(char));
                k = 0;
            }
            trans_iter++;
        }
    }
}

int orderTable(headerTable counter[15000], Ordered table[15000], int item_count)
{
    FILE *stream = fopen("data.txt", "r");
    if (stream == NULL)
    {
        printf("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    char ch;
    char *transItem = (char *)malloc(1000 * sizeof(char));
    strcpy(transItem, "\0");
    char *data_item = (char *)malloc(1000 * sizeof(char));

    int i = 0, j = 0, items_i = 0, last_index = -1, flag, seen1 = 0, seen2 = 0, totalTrans = 0, order_row = -1;
    while ((ch = fgetc(stream)) != EOF)
    {

        if (ch != ',' && ch != '\n')
        {
            if (seen1 != 0)
            {
                data_item[i] = ch;
                i++;
                seen2 = 1;
            }
        }

        else
        {
            if (ch == '\n')
            {
                totalTrans++;
                order_row++;
                if (totalTrans > 1)
                {
                    Ordering_Transactions(transItem, counter, table, order_row - 1, item_count);
                    free(transItem);
                    transItem = NULL;
                    transItem = (char *)malloc(10000 * sizeof(char));
                    strcpy(transItem, "\0");
                }
            }
            if (seen1 == 0)
            {
                seen1 = 1;
            }
            else if (seen2 == 1)
            {
                seen2 = 0;
                data_item[i] = ',';
                data_item[i + 1] = '\0';
                strcat(transItem, data_item);
                free(data_item);
                data_item = NULL;
                data_item = (char *)malloc(100 * sizeof(char));
                i = 0;
            }
        }
    }

    Ordering_Transactions(transItem, counter, table, order_row, item_count);
    free(transItem);
    transItem = NULL;

    fclose(stream);
    return totalTrans;
}

void build_baseFpTree(headerTable counter[], Ordered table[], int row_count)
{
    node *parent, *root, *curr, *tmp;
    root = (node *)malloc(sizeof(node));
    init_node(root);
    for (int i = 0; i < row_count; ++i)
    {
        curr = root;
        for (int j = 0; j < table[i].len; ++j)
        {
            int found = 0;
            parent = curr;
            curr = parent->child;
            if (curr == NULL && found == 0)
            {
                curr = (node *)malloc(sizeof(node));
                init_node(curr);
                curr->item = table[i].array[j];
                curr->count = 1;
                curr->parent = parent;
                parent->child = curr;
            }
            else
            {
                node **prev_l;
                tmp = curr;
                while (tmp != NULL && found == 0)
                {
                    if (tmp->item == table[i].array[j])
                    {
                        found = 1;
                        tmp->count++;
                        break;
                    }
                    prev_l = &(tmp->level);
                    tmp = tmp->level;
                }
                if (tmp == NULL)
                {
                    tmp = (node *)malloc(sizeof(node));
                    init_node(tmp);
                    tmp->item = table[i].array[j];
                    tmp->count = 1;
                    tmp->parent = parent;
                    *prev_l = tmp;
                }
                curr = tmp;
            }
            if (found == 0)
            {
                int l = curr->item;
                if (counter[l].addr == NULL)
                    counter[l].addr = curr;
                node **prev_sib_link = &(counter[l].prev->sibling);
                counter[l].prev = curr;
                if (counter[l].prev != counter[l].addr)
                    *prev_sib_link = curr;
            }
        }
    }
}

void buildFptree(miningTable miningCounter[], int miningCount, Ordered table[], int row_count)
{
    node *parent, *root, *curr, *tmp;
    root = (node *)malloc(sizeof(node));
    init_node(root);
    for (int i = 0; i < row_count; ++i)
    {
        curr = root;
        for (int j = 0; j < table[i].len; ++j)
        {
            int found = 0;
            parent = curr;
            curr = parent->child;
            if (curr == NULL && found == 0)
            {
                curr = (node *)malloc(sizeof(node));
                init_node(curr);
                curr->item = table[i].array[j];
                curr->count = table[i].count;
                curr->parent = parent;
                parent->child = curr;
            }
            else
            {
                node **prev_l;
                tmp = curr;
                while (tmp != NULL && found == 0)
                {
                    if (tmp->item == table[i].array[j])
                    {
                        found = 1;
                        tmp->count += table[i].count;
                        break;
                    }
                    prev_l = &(tmp->level);
                    tmp = tmp->level;
                }
                if (tmp == NULL)
                {
                    tmp = (node *)malloc(sizeof(node));
                    init_node(tmp);
                    tmp->item = table[i].array[j];
                    tmp->count = table[i].count;
                    tmp->parent = parent;
                    *prev_l = tmp;
                }
                curr = tmp;
            }
            if (found == 0)
            {
                int curr_item = curr->item;
                int l = find_index(curr_item, miningCounter, miningCount);

                if (miningCounter[l].addr == NULL)
                    miningCounter[l].addr = curr;
                node **prev_sib_link = &(miningCounter[l].prev->sibling);
                miningCounter[l].prev = curr;
                if (miningCounter[l].prev != miningCounter[l].addr)
                    *prev_sib_link = curr;
            }
        }
    }
}

void addToMinig(headerTable counter[], miningTable miningCounter[], int item_count)
{
    for (int i = 0; i <= item_count; ++i)
    {
        init_miningTable(&miningCounter[i]);
        miningCounter[i].item = i;
        miningCounter[i].count = counter[i].count;
        miningCounter[i].prev = counter[i].prev;
        miningCounter[i].addr = counter[i].addr;
    }
}
void mineFPtree(headerTable counter[], miningTable old_miningCounter[], int old_miningCount, Ordered old_table[], int old_row_count, int prefix[], int totalTrans)
{

    if (old_miningCount < 0)
        return;

    char prefix_string[1000] = "\0";
    int last_index = 0;

    while (prefix[last_index] != -1)
    {
        strcat(prefix_string, counter[prefix[last_index]].item);
        strcat(prefix_string, ",");
        last_index++;
    }

    for (int i = 0; i <= old_miningCount; ++i)
    {

        Ordered table[1000];
        miningTable miningCounter[1000];
        int new_prefix[1000];
        int miningCount = -1;
        int row_count = 0;

        int item = old_miningCounter[i].item;
        int item_total_count = old_miningCounter[i].count;

        for (int j = 0; j < last_index; ++j)
            new_prefix[j] = prefix[j];

        new_prefix[last_index] = item;
        new_prefix[last_index + 1] = -1;

        printf("transaction:: {%s%s} ||  Support :  %f%%\n", prefix_string, counter[item].item, ((float)old_miningCounter[i].count / totalTrans) * 100);

        node *curr_node, *curr_sibling;

        curr_sibling = old_miningCounter[i].addr;

        int k = 0;
        while (curr_sibling != NULL)
        {
            init_Ordered(&table[k]);
            int sibling_count = curr_sibling->count;
            table[k].count = sibling_count;

            curr_node = curr_sibling->parent;
            while (curr_node->parent != NULL)
            {
                int curr_item = curr_node->item;
                int found = 0;
                table[k].array[table[k].len++] = curr_item;

                for (int j = 0; j <= miningCount && found == 0; ++j)
                {
                    if (miningCounter[j].item == curr_item)
                    {
                        found = 1;
                        miningCounter[j].count += sibling_count;
                    }
                }

                if (found == 0)
                {
                    miningCount++;
                    init_miningTable(&miningCounter[miningCount]);
                    miningCounter[miningCount].item = curr_item;
                    miningCounter[miningCount].count = sibling_count;
                }
                curr_node = curr_node->parent;
            }
            curr_sibling = curr_sibling->sibling;
            k++;
        }
        row_count = k;
        int sup_count = supp;
        for (int j = 0; j < miningCount; ++j)
        {
            for (int k = j + 1; k <= miningCount; ++k)
            {
                if (miningCounter[j].count < miningCounter[k].count)
                {
                    int temp = miningCounter[j].item;
                    miningCounter[j].item = miningCounter[k].item;
                    miningCounter[k].item = temp;
                    temp = miningCounter[j].count;
                    miningCounter[j].count = miningCounter[k].count;
                    miningCounter[k].count = temp;
                }
            }
        }
        for (int j = 0; j < miningCount; ++j)
        {
            if (miningCounter[j].count < sup_count)
            {
                miningCount = j - 1;
                break;
            }
        }

        if (miningCount >= 0 && miningCounter[miningCount].count < sup_count)
            miningCount--;

        for (int k = 0; k < row_count; ++k)
        {
            int found = 0;
            int curr_index = 0;
            for (int j = 0; j <= miningCount; ++j)
            {
                for (int l = curr_index; l < table[k].len; ++l)
                {
                    if (table[k].array[l] == miningCounter[j].item)
                    {
                        if (l != curr_index)
                        {
                            int temp = table[k].array[l];
                            table[k].array[l] = table[k].array[curr_index];
                            table[k].array[curr_index] = temp;
                        }
                        curr_index++;
                    }
                }
            }
            table[k].len = curr_index;
        }
        buildFptree(miningCounter, miningCount, table, row_count);
        mineFPtree(counter, miningCounter, miningCount, table, row_count, new_prefix, totalTrans);
    }
}

int Assign(float sup)
{

    FILE *stream = fopen("data.txt", "r");
    if (stream == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    char ch;
    char *data_item = (char *)malloc(1000 * sizeof(char));
    headerTable counter[1000];
    miningTable miningCounter[10000];
    Ordered table[20000];
    init_table(&counter[0]);
    int i = 0, j = 0, items_i = 0, last_index = -1, flag, seen1 = 0, seen2 = 0, totalTrans = 0;

    while ((ch = fgetc(stream)) != EOF)
    {

        if (ch != ',' && ch != '\n')
        {
            if (seen1 != 0)
            {
                data_item[i] = ch;
                i++;
                seen2 = 1;
            }
        }

        else
        {
            if (ch == '\n')
                totalTrans++;
            if (seen1 == 0)
            {
                seen1 = 1;
            }
            else if (seen2 == 1)
            {
                seen2 = 0;
                data_item[i] = '\0';
                flag = 0;
                for (j = 0; j <= last_index && flag == 0; j++)
                {
                    if (strcmp(counter[j].item, data_item) == 0)
                    {
                        counter[j].count++;
                        flag = 1;
                        break;
                    }
                }
                if (flag == 0)
                {
                    last_index++;
                    init_table(&counter[last_index]);
                    counter[last_index].item = data_item;
                    counter[last_index].count = 1;
                }
                data_item = NULL;
                data_item = (char *)malloc(100 * sizeof(char));
                i = 0;
                if (ch == '\n')
                {
                    totalTrans++;
                    seen1 = 0;
                }
            }
        }
    }
    fclose(stream);

    sortDesc(counter, last_index + 1);

    for (int i = 0; i < last_index + 1; i++)
    {
        printf("%s\t%d\n", counter[i].item, counter[i].count);
    }
    supp = ceil(totalTrans * sup, 1);
    remove_infrequent(counter, &last_index);

    int row_count = orderTable(counter, table, last_index);
    build_baseFpTree(counter, table, row_count);
    addToMinig(counter, miningCounter, last_index);

    int prefix[1000];
    prefix[0] = -1;
    mineFPtree(counter, miningCounter, last_index, table, row_count, prefix, totalTrans);
    return 0;
}

int main()
{
    printf("-------------------------------\n");
    printf("Minmum Support : 20%%\n");
    Assign(0.2);
    // printf("-------------------------------\n");
    // printf("Minmum Support : 30%%\n");
    // Assign(0.3);
    // printf("-------------------------------\n");
    // printf("Minmum Support : 40%%\n");
    // Assign(0.4);
}