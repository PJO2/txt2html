# txt2html

A translator for UTF-8 characters to HTML coding.

```
Syntax: txt2html file.txt [file2.txt [file3.txt...]]
   or   txt2html *.txt
```

The program converts (some) non plain ASCII characters into their HTML equivalents
according tothe document [https://docs.moodle.org/405/en/HTML_entitiek](https://docs.moodle.org/405/en/HTML_entities)

Note the '<' and '>' characters are not converted to &lt; and &gt; to allow tags to remain intact in the .txt file.

For each .txt file processed, txt2html creates a corresponding .html file.


