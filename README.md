# txt2html

A translator for UTF-8 characters to HTML coding.

```
Syntax: txt2html [-dvq] file.txt [file2.txt [file3.txt...]]
   or   txt2html *.txt

   -d: add debugging information
   -v: more verbose (displays a warning for utf8 characters not translated)
   -q: quiet mode (displays only errors)
```

The program converts (some) non plain ASCII characters into their HTML equivalents
according tothe document [https://docs.moodle.org/405/en/HTML_entitiek](https://docs.moodle.org/405/en/HTML_entities)

Note the '<' and '>' characters are not converted to &amp;lt; and &amp;gt; to allow tags to remain intact in the .txt file.

For each .txt file processed, txt2html creates a corresponding .html file.

The .txt to .html translation can be automated thanks to inotifywait (part of the inotify-tools packages) using :
```
find [your directory] -name "*.txt" | 
  xargs inotifywait -r -m -e close_write | 
     while read file_path file_event file_name 
         do ~/C/txt2html/txt2html ${file_path}${file_name}
    done
```






