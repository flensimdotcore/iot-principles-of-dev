#include <stdio.h>

// Model
typedef struct 
{
    int data;
} Model;

void model_init(Model *model, int initial_data) 
{
    model->data = initial_data;
}

// View
void view_display(int data) 
{
    printf("Data from Model: %d\n", data);
}

// Controller
void controller_update(Model *model, int new_data) 
{
    model->data = new_data;
}

int main() 
{
    Model model;
    model_init(&model, 0);
    view_display(model.data);

    controller_update(&model, 1);
    view_display(model.data);

    return 0;
}
