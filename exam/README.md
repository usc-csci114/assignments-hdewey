## CSCI 114 Programming Exam Fall 2024

For this exam you will be using the following data set:

https://www.kaggle.com/datasets/mexwell/bug-attraction

Feel free to spend a few minutes looking at the Kaggle page, there are some helpful descriptions of the data and how it was collected.

The data was collected in an effort to understand how different lights attracted different insects. Each row gives the types (biological order) and numbers of bugs collected in a single trap. The traps were fitted with a particular type of light, there were 5 different light types. Various other pieces of data were recorded as well, including the phase of the moon.

Your task will be to create 3 different outputs, two graphs and one table from the data. Read through the full description so that you can think about the different variables and data structures you will need in your program.

#### Task 1: Data table

For this task make a table that shows for each species which light type is the most attractive. To make this calculation straightforward you do not need to consider any other factor besides ranking for each order the total number per light type.

Print out this table to the terminal formatted as nicely as possible.

#### Task 2: Scatter Plot

For this task we are interested in creating a scatter plot using the moon phase. You will make two plots: one for the urban "BG" site and one for the "LK" and "Stunt" sites. Choose appropriate variables and data structures to create a scatter plot of the total number of bugs for each row vs. the "Standardized Moon" for that row. 

Also on the same plot, draw the linear regression line for this data and add the line equation and R^2 value for the linear regression.

Make sure to include appropriate x and y labels on the graph.

#### Task 3: Bar Chart

For this task first find the 4 most common insect orders overall. Then create a grouped bar chart (see below) where each group is each light type and the 4 bars are the total for that light type for the 4 most common types overall. In the example below each month is a group, so in this problem the light type. Then the 4 shades are the 4 most common orders. Make sure to include appropriate group labels and a legend.

Think about how you can do this problem programmatically, i.e without hard-coding the 4 most common insect orders.

For this task the matplotplusplus documentation on bar plots will be helpful:

https://alandefreitas.github.io/matplotplusplus/plot-types/discrete-data/bar-plot/