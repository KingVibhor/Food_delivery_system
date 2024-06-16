#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_RESTAURANTS 4
#define MAX_LOCATIONS 10

typedef struct{
  char locationName[50];
}Location;

typedef struct MenuItem {
    char name[50];
    float price;
    int id;
} MenuItem;

typedef struct OrderItem {
    int menuItemId;
    int quantity;
} OrderItem;

typedef struct Order {
    int orderId;
    char customerName[50];
    OrderItem* items;
    int numItems;
    int restaurantId;
    int locationId;
    int locationIndex;
    float total;
    float deliveryCharges;
    char deliveryAddress[100];
    struct Order* next;  
} Order;

typedef struct AVLNode {
    int key;
    Order* order;
    int height;
    struct AVLNode* left;
    struct AVLNode* right;
} AVLNode;

typedef struct Restaurant {
    char name[50];
    MenuItem* menu;
    int menuSize;
    AVLNode* orders;
    Order* orderListHead;
    Location locations[MAX_LOCATIONS];
} Restaurant;

Restaurant restaurants[MAX_RESTAURANTS] = {
    {"Vaishali", NULL, 0, NULL, NULL, {
        {"Vaishali"}, {"Katrej"}, {"Kothrud"}, {"Viman Nagar"}, {"Hadapsar"}, {"Baner"}, {"PCMC"}, {"Lohegaon"}, {"Shivaji Nagar"}, {"SPPU"}
    }},
    {"Camp Burger", NULL, 0, NULL, NULL, {
        {"Camp Burger"}, {"Katrej"}, {"Kothrud"}, {"Viman Nagar"}, {"Hadapsar"}, {"Baner"}, {"PCMC"}, {"Lohegaon"}, {"Shivaji Nagar"}, {"SPPU"}
    }},
    {"Singh Saab Paratha House", NULL, 0, NULL, NULL, {
        {"Singh Saab Paratha House"}, {"Katrej"}, {"Kothrud"}, {"Viman Nagar"}, {"Hadapsar"}, {"Baner"}, {"PCMC"}, {"Lohegaon"}, {"Shivaji Nagar"}, {"SPPU"}
    }},
    {"The Sushi Factory", NULL, 0, NULL, NULL, {
        {"The Sushi Factory"}, {"Katrej"}, {"Kothrud"}, {"Viman Nagar"}, {"Hadapsar"}, {"Baner"}, {"PCMC"}, {"Lohegaon"}, {"Shivaji Nagar"}, {"SPPU"}
    }}
};

void displayRestaurants();
void displayMenu(int restaurantId);
int dijkstra(int distances[MAX_LOCATIONS][MAX_LOCATIONS], int src, int dest);
int primMST(int graph[MAX_LOCATIONS][MAX_LOCATIONS], int src, int vertices);

int Graph[MAX_RESTAURANTS][MAX_LOCATIONS][MAX_LOCATIONS] = {
    { // Graph for Restaurant 0
        {0, 8, 5, 12, 13, 11, 16, 15, 3, 4},
        {8, 0, 12, 18, 14, 22, 23, 22, 10, 11},
        {5, 12, 0, 16, 16, 11, 20, 20, 6, 9},
        {12, 18, 16, 0, 9, 20, 20, 6, 11, 16},
        {13, 14, 16, 9, 0, 20, 12, 21, 8, 17},
        {11, 22, 11, 20, 20, 0, 14, 22, 13, 8},
        {16, 23, 20, 20, 12, 14, 0, 22, 13, 15},
        {15, 22, 20, 6, 21, 22, 22, 0, 14, 16},
        {3, 10, 6, 11, 8, 13, 13, 14, 0, 4},
        {4, 11, 9, 16, 17, 18, 15, 16, 4, 0}
    },
    { // Graph for Restaurant 1
        {0, 9, 10, 10, 6, 15, 18, 14, 6, 11},
        {9, 0, 12, 18, 14, 22, 23, 22, 10, 11},
        {10, 12, 0, 16, 16, 11, 20, 20, 6, 9},
        {10, 18, 16, 0, 9, 20, 20, 6, 11, 16},
        {6, 14, 16, 9, 0, 20, 12, 21, 8, 17},
        {15, 22, 11, 20, 20, 0, 14, 22, 13, 8},
        {18, 23, 20, 20, 12, 14, 0, 22, 13, 15},
        {14, 22, 20, 6, 21, 22, 22, 0, 14, 16},
        {6, 10, 6, 11, 8, 13, 13, 14, 0, 4},
        {11, 11, 9, 16, 17, 18, 15, 16, 4, 0}
    },
    { // Graph for Restaurant 2
        {0, 13, 1, 15, 15, 13, 19, 19, 5, 8},
        {13, 0, 12, 18, 14, 22, 23, 22, 10, 11},
        {1, 12, 0, 16, 16, 11, 20, 20, 6, 9},
        {15, 18, 16, 0, 9, 20, 20, 6, 11, 16},
        {15, 14, 16, 9, 0, 20, 12, 21, 8, 17},
        {13, 22, 11, 20, 20, 0, 14, 22, 13, 8},
        {19, 23, 20, 20, 12, 14, 0, 22, 13, 15},
        {19, 22, 20, 6, 21, 22, 22, 0, 14, 16},
        {5, 10, 6, 11, 8, 13, 13, 14, 0, 4},
        {8, 11, 9, 16, 17, 18, 15, 16, 4, 0}
    },
    { // Graph for Restaurant 3
        {0, 12, 2, 15, 14, 14, 18, 7, 4, 4},
        {12, 0, 12, 18, 14, 22, 23, 22, 10, 11},
        {2, 12, 0, 16, 16, 11, 20, 20, 6, 9},
        {15, 18, 16, 0, 9, 20, 20, 6, 11, 16},
        {14, 14, 16, 9, 0, 20, 12, 21, 8, 17},
        {14, 22, 11, 20, 20, 0, 14, 22, 13, 8},
        {18, 23, 20, 20, 12, 14, 0, 22, 13, 15},
        {7, 22, 20, 6, 21, 22, 22, 0, 14, 16},
        {4, 10, 6, 11, 8, 13, 13, 14, 0, 4},
        {4, 11, 9, 16, 17, 18, 15, 16, 4, 0}
    }
};

long long orderIdCounter = 1;

int height(AVLNode* node) {
    if (node == NULL)
        return 0;
    return node->height;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

AVLNode* minValueNode(AVLNode* node) {
    AVLNode* current = node;

    while (current && current->left != NULL)
        current = current->left;

    return current;
}


AVLNode* rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

AVLNode* leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

int getBalance(AVLNode* N) {
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

int minKey(int key[], int mstSet[], int vertices) {
    int min = INT_MAX, min_index;

    for (int v = 0; v < vertices; v++)
        if (mstSet[v] == 0 && key[v] < min) {
            min = key[v];
            min_index = v;
        }

    return min_index;
}


AVLNode* insertAVL(AVLNode* node, Order* order, Order** listHead) {
    if (node == NULL) {
        AVLNode* newNode = (AVLNode*)malloc(sizeof(AVLNode));
        newNode->order = order;
        newNode->key = order->orderId;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->height = 1;  

        order->next = *listHead;
        *listHead = order;

        return newNode;
    }

    if (order->orderId < node->order->orderId)
        node->left = insertAVL(node->left, order, listHead);
    else if (order->orderId > node->order->orderId)
        node->right = insertAVL(node->right, order, listHead);
    else{ 
        return node;
    }
  
    node->height = 1 + max(height(node->left), height(node->right));


    int balance = getBalance(node);

    // Left Left Case
    if (balance > 1 && order->orderId < node->left->order->orderId)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && order->orderId > node->right->order->orderId)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && order->orderId > node->left->order->orderId) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && order->orderId < node->right->order->orderId) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

AVLNode* deleteNode(AVLNode* root, int key, Order** listHead) {
    if (root == NULL)
        return root;


    if (key < root->key){
        root->left = deleteNode(root->left, key, listHead);
    }else if (key > root->key){
        root->right = deleteNode(root->right, key, listHead);
    }else {
        Order **tempLink = listHead;
        while(*tempLink && (*tempLink)->orderId != key){
            tempLink = &(*tempLink)->next;
        }
        if(*tempLink){
            Order* tempOrder = *tempLink;
            *tempLink = (*tempLink)->next;
            free(tempOrder);
        }
        if ((root->left == NULL) || (root->right == NULL)) {
            AVLNode *temp = root->left ? root->left : root->right;

            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else {
                *root = *temp;
            }

            free(temp);
        } else {
            AVLNode* temp = minValueNode(root->right);
            root->key = temp->key;
            root->order = temp->order;
            root->right = deleteNode(root->right, temp->key, listHead);
        }
    }

    if (root == NULL)
        return root;

    root->height = 1 + max(height(root->left), height(root->right));


    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

AVLNode* searchAVL(AVLNode* root, int orderId) {

    if (root == NULL || root->key == orderId) {
        return root;
    }

    if (orderId > root->key) {
        return searchAVL(root->right, orderId);
    }

    return searchAVL(root->left, orderId);
}

int primMST(int graph[MAX_LOCATIONS][MAX_LOCATIONS], int src, int vertices) {
    int parent[MAX_LOCATIONS];
    int key[MAX_LOCATIONS];    
    int mstSet[MAX_LOCATIONS]; 
    int totalCost = 0;

    for (int i = 0; i < vertices; i++) {
        key[i] = INT_MAX;
        mstSet[i] = 0;
    }

 
    key[src] = 0;     
    parent[src] = -1; 


    for (int count = 0; count < vertices - 1; count++) {

        int u = minKey(key, mstSet, vertices);

        mstSet[u] = 1;

        for (int v = 0; v < vertices; v++) {

            if (graph[u][v] && mstSet[v] == 0 && graph[u][v] < key[v]) {
                parent[v] = u, key[v] = graph[u][v];
            }
        }
    }

    for (int i = 1; i < vertices; i++) {
        totalCost += graph[i][parent[i]];
    }

    return totalCost;
}

void placeOrder() {
    char customerName[50];
    int restaurantId, numItems, itemId, quantity, i, locationId;
    float total = 0;

    printf("Enter customer name: ");
    scanf("%s", &customerName);

    displayRestaurants();
    printf("Select a restaurant (enter ID): ");
    scanf("%d", &restaurantId);
    while(getchar() != '\n');

    if(restaurantId < 0 || restaurantId >= MAX_RESTAURANTS){
        printf("Invalid restaurant ID.\n");
        return;
    }

    displayMenu(restaurantId); 
    
    printf("Locations where we deliver:\n");
    for (i = 1; i < MAX_LOCATIONS; i++) {
        printf("%d. %s\n", i, restaurants[restaurantId].locations[i].locationName);
    }
    printf("Select a delivery location (enter ID): ");
    scanf("%d", &locationId);
    while(getchar() != '\n');
    if (locationId <= 0 || locationId >= MAX_LOCATIONS) {
        printf("Invalid location ID. Please enter a number between 0 and %d.\n", MAX_LOCATIONS - 1);
        return;
    }

    printf("No. of orders: ");
    scanf("%d", &numItems);

    OrderItem* items = (OrderItem*)malloc(sizeof(OrderItem) * numItems);
    if(items == NULL){
        printf("Memory allocation failed.\n");
        return;
    }

    for (i = 0; i < numItems; i++) {
        printf("Enter item id and quantity (e.g., 1 2): ");
        scanf("%d %d", &itemId, &quantity);
        if (itemId < 0 || itemId >= restaurants[restaurantId].menuSize) {
            printf("Invalid menu item ID.\n");
            free(items);
            return;
        }
        items[i].menuItemId = itemId;
        items[i].quantity = quantity;
        total += restaurants[restaurantId].menu[itemId].price * quantity;
    }

    Order* newOrder = (Order*)malloc(sizeof(Order));
    if (newOrder == NULL) {
        printf("Memory allocation failed.\n");
        free(items);
        return;
    }

    strcpy(newOrder->customerName, customerName);
    newOrder->orderId = orderIdCounter++;
    newOrder->items = items;
    newOrder->numItems = numItems;
    newOrder->total = total;
    newOrder->locationIndex = locationId;

    restaurants[restaurantId].orders = insertAVL(restaurants[restaurantId].orders, newOrder, &restaurants[restaurantId].orderListHead);
    
    int mstCost = primMST(Graph[restaurantId], 0, MAX_LOCATIONS);
    int shortestDistance = dijkstra(Graph[restaurantId], 0, locationId);  
    float deliveryCharges = mstCost * 0.5;  // Rs.0.5 per km
    
    newOrder->deliveryCharges = deliveryCharges;

    printf("Shortest distance from %s to %s is %d km.\n",
           restaurants[restaurantId].locations[0].locationName,
           restaurants[restaurantId].locations[locationId].locationName,
           shortestDistance);

    float totalBill = total + deliveryCharges;
printf("Order ID: %d\n", newOrder->orderId);
printf("Restaurant ID : %d\n", restaurantId);
printf("Order Total: Rs. %.2f\n", total);
printf("Delivery Charges : Rs. %.2f\n", shortestDistance, deliveryCharges);
printf("Total Bill (including delivery charges): Rs. %.2f\n", totalBill);
}


void cancelOrder() {
    int orderId;
    int restaurantId;
    printf("Enter Restaurant ID: ");
    scanf("%d", &restaurantId);
        if(restaurantId > restaurantId){
        printf("Please enter order Id");
    }else{
    printf("Enter Order ID to cancel: ");
    scanf("%d", &orderId);
        if(orderId > orderIdCounter){
        printf("Please enter correct order Id");
    }else{
    restaurants[restaurantId].orders = deleteNode(restaurants[restaurantId].orders, orderId, &restaurants[restaurantId].orderListHead);
    printf("Order %d has been cancelled.\n", orderId);
    }
}
}

void searchOrder() {
    int orderId;
    int restaurantId;
    printf("Enter Restaurant ID where to search: ");
    scanf("%d", &restaurantId);
    while(getchar() != '\n');

    if(restaurantId >= MAX_RESTAURANTS){
        printf("Please enter correct Id");
    }else{
    printf("Enter Order ID to search: ");
    scanf("%d", &orderId);
    while(getchar() != '\n');

     if(orderId > orderIdCounter){
        printf("Please enter correct order Id");
    }else{

    AVLNode* result = searchAVL(restaurants[restaurantId].orders, orderId);

    if (result == NULL) {
        printf("Order ID %d not found.\n", orderId);
    } else {
        printf("Order ID %d found. \nDetails:\n", orderId);
        printf("Order ID: %d\n", result->order->orderId);
        printf("Restaurant ID: %d\n", restaurantId);
        printf("Customer Name: %s\n", result->order->customerName);
        printf("Total Bill (including delivery charges): Rs. %.2f\n", result->order->total + result->order->deliveryCharges);
        printf("Delivery Location: %s\n", restaurants[restaurantId].locations[result->order->locationIndex].locationName);
     }
    }
    }
}

int dijkstra(int distances[MAX_LOCATIONS][MAX_LOCATIONS], int src, int dest) {
    int dist[MAX_LOCATIONS];
    bool sptSet[MAX_LOCATIONS];

    for (int i = 0; i < MAX_LOCATIONS; i++) {
        dist[i] = INT_MAX;
        sptSet[i] = false;
    }

    dist[src] = 0;

    for (int count = 0; count < MAX_LOCATIONS - 1; count++) {
        int u = -1;

        for (int v = 0; v < MAX_LOCATIONS; v++)
            if (!sptSet[v] && (u == -1 || dist[v] < dist[u]))
                u = v;

        sptSet[u] = true;

        for (int v = 0; v < MAX_LOCATIONS; v++) {
            if (!sptSet[v] && distances[u][v] && dist[u] != INT_MAX &&
                dist[u] + distances[u][v] < dist[v])
                dist[v] = dist[u] + distances[u][v];
        }
    }

    return dist[dest]; 
}

void initializeMenus() {
    // Restaurant 0
    MenuItem items0[] = {
        {"Masala Dosa", 80, 0},
        {"Onion Uttappa", 95, 1},
        {"Cheese Sandwich", 100, 2}
    };
    restaurants[0].menu = malloc(sizeof(items0));
    memcpy(restaurants[0].menu, items0, sizeof(items0));
    restaurants[0].menuSize = sizeof(items0) / sizeof(MenuItem);
    strcpy(restaurants[0].name, "Vaishali");

    // Restaurant 1
    MenuItem items1[] = {
        {"Veg Burger", 80, 0},
        {"Chicken Burger", 120, 1},
        {"French Fries", 60, 2}
    };
    restaurants[1].menu = malloc(sizeof(items1));
    memcpy(restaurants[1].menu, items1, sizeof(items1));
    restaurants[1].menuSize = sizeof(items1) / sizeof(MenuItem);
    strcpy(restaurants[1].name, "Camp Burger");

    // Restaurant 2
    MenuItem items2[] = {
        {"Aloo Paratha", 105, 0},
        {"Paneer Paratha", 135, 1},
        {"Mushroom Cheese Paratha", 165, 2}
    };
    restaurants[2].menu = malloc(sizeof(items2));
    memcpy(restaurants[2].menu, items2, sizeof(items2));
    restaurants[2].menuSize = sizeof(items2) / sizeof(MenuItem);
    strcpy(restaurants[2].name, "Singh Saab Paratha House");

    // Restaurant 3
    MenuItem items3[] = {
        {"Tofu California Sushi", 220, 0},
        {"Chicken Ramen", 370, 1},
        {"Prawns Dumplings", 170, 2}
    };
    restaurants[3].menu = malloc(sizeof(items3));
    memcpy(restaurants[3].menu, items3, sizeof(items3));
    restaurants[3].menuSize = sizeof(items3) / sizeof(MenuItem);
    strcpy(restaurants[3].name, "The Sushi Factory");
}

void displayRestaurants() {
    printf("\nAvailable Restaurants:\n");
    printf("-----------------------------------\n");
    printf("ID   Restaurant Name\n");
    printf("-----------------------------------\n");
    for (int i = 0; i < MAX_RESTAURANTS; i++) {  
        printf("%-4d %s\n", i, restaurants[i].name);
    }
    printf("-----------------------------------\n");
}


void displayMenu(int restaurantId) {
    printf("\nMenu for %s:\n", restaurants[restaurantId].name);
    printf("--------------------------------\n");
    printf("ID   Item Name        Price\n");
    printf("--------------------------------\n");

    for (int i = 0; i < restaurants[restaurantId].menuSize; i++) {
        printf("%-4d %-15s Rs.%.2f\n", 
               restaurants[restaurantId].menu[i].id, 
               restaurants[restaurantId].menu[i].name, 
               restaurants[restaurantId].menu[i].price);
    }
    printf("--------------------------------\n");
}

int main() {
    int choice;
    initializeMenus();  

    while (1) {
        printf("Menu\n");
        printf("1. Place an order\n");
        printf("2. Search your order\n");
        printf("3. Cancel your order\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                placeOrder();
                break;
            case 2:
                searchOrder();
                break;
            case 3:
                cancelOrder();
                break;
            case 4:
                printf("Exiting program. \n");
                exit(0);
            default:
                printf("Invalid choice. Please enter again.\n");
        }
    }
    return 0;
}
