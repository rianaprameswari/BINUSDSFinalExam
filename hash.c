#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 3
#define MIN 2

typedef struct {
    int id;
    char name[100];
    char category[50];
    float price;
    int stock;
    char description[200];
} Product;

typedef struct btreeNode {
    int n;  //Jumlah keys
    Product keys[MAX + 1];  
    struct btreeNode *child[MAX + 1];  // Array pointer child
    int leaf;  // node leaf true
} btreeNode;

btreeNode *root = NULL;

btreeNode *NodeBaru(int leaf) {
    btreeNode *newNode = (btreeNode *)malloc(sizeof(btreeNode));
    newNode->n = 0;
    newNode->leaf = leaf;
    for (int i = 0; i < MAX + 1; i++)
        newNode->child[i] = NULL;
    return newNode;
}

void splitChild(btreeNode *parent, int i, btreeNode *child) {
    btreeNode *newChild = NodeBaru(child->leaf);
    newChild->n = MIN;

    for (int j = 0; j < MIN; j++)
        newChild->keys[j] = child->keys[j + MIN + 1];

    if (!child->leaf) {
        for (int j = 0; j < MIN + 1; j++)
            newChild->child[j] = child->child[j + MIN + 1];
    }

    child->n = MIN;

    for (int j = parent->n; j >= i + 1; j--)
        parent->child[j + 1] = parent->child[j];

    parent->child[i + 1] = newChild;

    for (int j = parent->n - 1; j >= i; j--)
        parent->keys[j + 1] = parent->keys[j];

    parent->keys[i] = child->keys[MIN];
    parent->n++;
}

void insertNF(btreeNode *node, Product k) {
    int i = node->n - 1;

    if (node->leaf) {
        while (i >= 0 && k.id < node->keys[i].id) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = k;
        node->n++;
    } else {
        while (i >= 0 && k.id < node->keys[i].id)
            i--;
        i++;
        if (node->child[i]->n == MAX) {
            splitChild(node, i, node->child[i]);
            if (k.id > node->keys[i].id)
                i++;
        }
        insertNF(node->child[i], k);
    }
}

void insertProduct(Product k) {
    if (root == NULL) {
        root = NodeBaru(1);
        root->keys[0] = k;
        root->n = 1;
    } else {
        if (root->n == MAX) {
            btreeNode *newRoot = NodeBaru(0);
            newRoot->child[0] = root;
            splitChild(newRoot, 0, root);
            int i = 0;
            if (newRoot->keys[0].id < k.id)
                i++;
            insertNF(newRoot->child[i], k);
            root = newRoot;
        } else {
            insertNF(root, k);
        }
    }
}
void traverse(btreeNode *node) {
    int i;
    for (i = 0; i < node->n; i++) {
        if (!node->leaf)
            traverse(node->child[i]);
        printf("Product ID: %d, Nama: %s\n", node->keys[i].id, node->keys[i].name);
    }
    if (!node->leaf)
        traverse(node->child[i]);
}

Product *BtreeSearch(btreeNode *node, int id) {
    int i = 0;
    while (i < node->n && id > node->keys[i].id)
        i++;
    if (i < node->n && node->keys[i].id == id)
        return &node->keys[i];
    if (node->leaf)
        return NULL;
    return BtreeSearch(node->child[i], id);
}

void deleteLeaf(btreeNode *node, int idx) {
    for (int i = idx + 1; i < node->n; i++)
        node->keys[i - 1] = node->keys[i];
    node->n--;
}

void deleteNonLeaf(btreeNode *node, int idx) {
    Product k = node->keys[idx];

    if (node->child[idx]->n >= MIN + 1) {
        btreeNode *cur = node->child[idx];
        while (!cur->leaf)
            cur = cur->child[cur->n];
        Product pred = cur->keys[cur->n - 1];
        node->keys[idx] = pred;
        deleteNode(node->child[idx], pred.id);
    } else if (node->child[idx + 1]->n >= MIN + 1) {
        btreeNode *cur = node->child[idx + 1];
        while (!cur->leaf)
            cur = cur->child[0];
        Product succ = cur->keys[0];
        node->keys[idx] = succ;
        deleteNode(node->child[idx + 1], succ.id);
    } else {
        btreeNode *child = node->child[idx];
        btreeNode *sibling = node->child[idx + 1];
        child->keys[MIN] = k;
        for (int i = 0; i < sibling->n; i++)
            child->keys[i + MIN + 1] = sibling->keys[i];
        if (!child->leaf) {
            for (int i = 0; i <= sibling->n; i++)
                child->child[i + MIN + 1] = sibling->child[i];
        }
        for (int i = idx + 1; i < node->n; i++)
            node->keys[i - 1] = node->keys[i];
        for (int i = idx + 2; i <= node->n; i++)
            node->child[i - 1] = node->child[i];
        child->n += sibling->n + 1;
        node->n--;
        free(sibling);
        deleteNode(child, k.id);
    }
}
// hapus function
void deleteNode(btreeNode *node, int id) {
    int idx = 0;
    while (idx < node->n && node->keys[idx].id < id)
        idx++;
    if (idx < node->n && node->keys[idx].id == id) {
        if (node->leaf)
            deleteLeaf(node, idx);
        else
            deleteNonLeaf(node, idx);
    } else {
        if (node->leaf) {
            printf("Product ID %d tidak ada.\n", id);
            return;
        }
        int flag = (idx == node->n) ? 1 : 0;
        if (node->child[idx]->n < MIN + 1) {
            if (idx != 0 && node->child[idx - 1]->n >= MIN + 1) {
                btreeNode *child = node->child[idx];
                btreeNode *sibling = node->child[idx - 1];
                for (int i = child->n - 1; i >= 0; i--)
                    child->keys[i + 1] = child->keys[i];
                if (!child->leaf) {
                    for (int i = child->n; i >= 0; i--)
                        child->child[i + 1] = child->child[i];
                }
                child->keys[0] = node->keys[idx - 1];
                if (!child->leaf)
                    child->child[0] = sibling->child[sibling->n];
                node->keys[idx - 1] = sibling->keys[sibling->n - 1];
                child->n++;
                sibling->n--;
            } else if (idx != node->n && node->child[idx + 1]->n >= MIN + 1) {
                btreeNode *child = node->child[idx];
                btreeNode *sibling = node->child[idx + 1];
                child->keys[child->n] = node->keys[idx];
                if (!child->leaf)
                    child->child[child->n + 1] = sibling->child[0];
                node->keys[idx] = sibling->keys[0];
                for (int i = 1; i < sibling->n; i++)
                    sibling->keys[i - 1] = sibling->keys[i];
                if (!sibling->leaf) {
                    for (int i = 1; i <= sibling->n; i++)
                        sibling->child[i - 1] = sibling->child[i];
                }
                child->n++;
                sibling->n--;
            } else {
                if (idx != node->n) {
                    btreeNode *child = node->child[idx];
                    btreeNode *sibling = node->child[idx + 1];
                    child->keys[MIN] = node->keys[idx];
                    for (int i = 0; i < sibling->n; i++)
                        child->keys[i + MIN + 1] = sibling->keys[i];
                    if (!child->leaf) {
                        for (int i = 0; i <= sibling->n; i++)
                            child->child[i + MIN + 1] = sibling->child[i];
                    }
                    for (int i = idx + 1; i < node->n; i++)
                        node->keys[i - 1] = node->keys[i];
                    for (int i = idx + 2; i <= node->n; i++)
                        node->child[i - 1] = node->child[i];
                    child->n += sibling->n + 1;
                    node->n--;
                    free(sibling);
                } else {
                    btreeNode *child = node->child[idx];
                    btreeNode *sibling = node->child[idx - 1];
                    sibling->keys[MIN] = node->keys[idx - 1];
                    for (int i = 0; i < child->n; i++)
                        sibling->keys[i + MIN + 1] = child->keys[i];
                    if (!sibling->leaf) {
                        for (int i = 0; i <= child->n; i++)
                            sibling->child[i + MIN + 1] = child->child[i];
                    }
                    node->n--;
                    sibling->n += child->n + 1;
                    free(child);
                }
            }
        }
        if (flag && idx > node->n)
            deleteNode(node->child[idx - 1], id);
        else
            deleteNode(node->child[idx], id);
    }
}

void delete(int id) {
    if (root == NULL) {
        printf("Tree kosong.\n");
        return;
    }
    deleteNode(root, id);
    if (root->n == 0) {
        btreeNode *tmp = root;
        if (root->leaf)
            root = NULL;
        else
            root = root->child[0];
        free(tmp);
    }
}

int main() {
    int choice, id, stock;
    float price;
    char name[100], category[50], description[200];
    Product *foundProduct;

    while (1) {
        printf("1. Masukkan Product\n");
        printf("2. Cari Produk\n");
        printf("3. Hapus Produk\n");
        printf("4. Exit\n");
        printf("MAsukkan Pilihan menu: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Masukkan Product ID: ");
                scanf("%d", &id);
                printf("Masukkan Product Nama: ");
                scanf("%s", name);
                printf("Masukkan Kategori Product : ");
                scanf("%s", category);
                printf("Masukkan HArga: ");
                scanf("%f", &price);
                printf("Masukkan Jumlah Stok: ");
                scanf("%d", &stock);
                printf("Masukkan Deskripsi Produk: ");
                scanf(" %[^\n]", description); 

                Product p = {id, "", "", price, stock, ""};
                strcpy(p.name, name);
                strcpy(p.category, category);
                strcpy(p.description, description);

                insertProduct(p);
                printf("Product inserted successfully.\n");
                break;

            case 2:
                printf("Masukkan Product ID to BtreeSearch: ");
                scanf("%d", &id);
                foundProduct = BtreeSearch(root, id);
                if (foundProduct != NULL)
                    printf("Product found: ID=%d, Nama=%s, Kategori=%s, Harga=%.2f, Stock=%d, Deskripsi=%s\n",
                           foundProduct->id, foundProduct->name, foundProduct->category, foundProduct->price, foundProduct->stock, foundProduct->description);
                else
                    printf("Product ID %d tidak ditemukan.\n", id);
                break;

            case 3:
                printf("Masukkan Product ID dihapus : ");
                scanf("%d", &id);
                delete(id);
                printf("Product berhasil di delete .\n");
                break;

            case 4:
                exit(0);

            default:
                printf("Input salah.\n");
        }
    }

    return 0;
}
