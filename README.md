# virt_mem_tree_visualizer

pet-project to visualize graph of virtual memory multilevel page tables (https://en.wikipedia.org/wiki/Page_table#Multilevel_page_tables).

## Properties

### pagemap
You will need pagemap (https://github.com/dwks/pagemap).

Install it 
~~~bash
> git clone https://github.com/dwks/pagemap
~~~
and add to PATH. As long as pagemap requires **sudo** for correct page frame number output you need to add it to sudo PATH.

~~~bash
> sudo visudo
~~~

You need to change this line
~~~
Defaults secure_path = [something-you-do-no-need-to-change]:<path-to-pagemap>
~~~

## Build

~~~bash
> make
~~~

## Usage

To generate a tree.json

~~~bash
> bin/mem_tree_by_pid <PID>
~~~

Then to visualize it

~~~bash
> python3 treegen.py
~~~
