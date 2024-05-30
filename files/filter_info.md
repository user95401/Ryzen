# Filters... yea

One by one filter key is separating by <cr>`&`</c>.

#### Filter key can be as:
- any text to find in whole json data dump of issue
- any json key, like `labels`, `title`, `locked`, `created_at`, `updated_at`

Filter key formatted as key, separator <cr>`:`</c> or <cr>`~:`</c>, and value to find. (<co>key</c><cy>:</c><cr>value</c>)
```
title:The title
```
```
the_any_word
```

Here can be multiple values to search. Separate them with <cr>`,`</c>

Like: `user:shyFox,fineFox,shineFox`.

Here is filter key variants for <cr>all `:`</c> or <co>any `~:`</c>.

Also it's better to <cg>put them in quotes</c>, <co>especially for labels</c>.
```
labels:"mod","hack"
```

## Examples:

Filter for hack themes mods by user95401:
```
user:"user95401"&labels:"mod","hack"
```

Filter for packs with name that contains "icon":
```
labels:"mod","hack"&title:icon
```

Filter any mods that posted at May 2024:
```
labels:"mod"&created_at:2024-05
```
Or ever on May 26...
```
labels:"mod"&created_at:05-26
```