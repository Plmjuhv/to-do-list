# To Do List
A terminal based to do list to help keep track of tasks that you must do  

Creates a linked list of tasks upon which multiple different commands can be performed  

## Commands
- Add a task to the list with a category and a priority which is one of low, medium, or high  
a [task] [category] [priority]

- Print out the current task list  
p

- Update the priority of a task, low -> medium, medium -> high, high -> low  
i [task] [category]

- Count the amount of tasks in the list  
n

- Move a task from the tasks list to the completed tasks list, if the start time is listed as -1 then it will be equal to the finish time of the task in the completed tasks with the latest finish time  
c [task] [category] [start_time (minutes)] [finish_time (minutes)]

- Print the list of completed tasks
P

- Estimate the required completion time for uncompleted tasks based on the amount of time taken for completed tasks of the same category, if none exist then set to 100 minutes  
e

- Delete a task from the tasks list, not the completed tasks list  
d [task] [category]

- Complete a day of tasks and free the completed tasks  
f

- Mark a task as repeatable, any repeatable tasks will be moved back to the tasks list from the completed tasks list when a day is ended  
r [task] [category]

- Match tasks based on a given name pattern and return a list of all matching tasks with their information  
 m [pattern]
  - \* matches any amount of any character in the name
  - ? matches a single character of any kind
  - [abc] matches any of the characters in the square brackets to one single character
  - abc matches the exact characters in the exact order
    - E.g. [ea1op]xa*l? would match with example  

- Delete tasks based on matched patterns in names  
^ [pattern]

- Sort tasks with the priority order of category -> priority -> name
s
