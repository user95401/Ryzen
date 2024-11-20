# Commands
Every issue comment is processed by workflow for commands.

- <cp>`!setlabels`</c> <ca>`label`</c><cy>,</c><ca>`other-label`</c> to <cj>toggle label</c>.
```
ONLY issue creator can use that, 
"verified" and "featured" labels causes cancel WHOLE command.
```
- <cp>`!actualversion`</c><cy>=</c><ca>`v1.1.0`</c> to <cj>set actual version</c>. 
```
Its will be saved at user locally.

Mod crawling for latest comment contains "!actualversion=".
And will notify if rvalue is not founded in local user save.

All downloaded versions is saved in 
"geode/ryzen/mods/yourmod/.downloadedVersions"
```
