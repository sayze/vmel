# Vmel Grammar

The below outlines the grammar rules behind vmel. 


## Assignment
A typical *Assignment* production will consist of the below rules. An assignment may either be a STRING or expression. Examples such as

`$name = "Smith"`

`$age = 33` 

`$income = 166 + 23 * 4`

And the corresponding grammar.


```
assignment = expr | STRING
expr = term | term (+ | -) term
term = factor | factor (* | /) factor
factor = INTEGER
```

## Groups
A *Group* production is fairly trivial in comparison to an *Assignment*. It simply comprises of a group name (identifier) followed by a list of commands pertaining to that group.
Examples such as

```
{enable_apache}
"sudo service apache2 enable"
... so on
```
And the corresponding grammar.
```
Group = { valid_idn } NEWLINE command_list
valid_idn = a-zA-Z | - | _
command_list = command NEWLINE
command = STRING
```