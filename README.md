Edit the Makefile, change the path to the NCL library if necessary and run ``make``.

To convert an existing Nexus file, run

```
nexus2phyjson file.nex > file.phyjson
```

The PhyJSON format is described in doc/phyjson_format_description.md.

Notes:

* Only the first taxa block is used.
* Only the first tree in the first tree block is used.
* Export of characters is not yet supported.
