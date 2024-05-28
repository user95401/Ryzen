# Filters... yea

One by one filter key is seperating by <cg>`&`</c>.

#### List:
- `name:` - the title of issue
- `by:` - the name of user that created issue
- `labels:` - labels like `mod`, `hack`, `ui`. You can seperate them with <cg>`,`</c>
- any word to find in whole json data dump of issue

## Examples:
#### Filter for hack themed mods by user95401:
```
by:user95401&labels:mod,hack
```
#### Filter for packs with name that contains "icon":
```
labels:mod,hack&name:icon
```

#### Filter any mods that posted at May 2024:
i take part from json str `"created_at": "2024-05`
```
"created_at": "2024-05&labels:mod
```
