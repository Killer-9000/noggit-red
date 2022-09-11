
Get project version from CurrentProject -> NoggitProject -> projectVersion

Model pipeline:
 - Loader        - version specific
 - Data class    - version specific
 - Generic class
 - Renderer      - version specific

Data pipeline:
 - Select project
 - Select loader based on version
 - Set as loader

DBC pipeline:
 - Select DBC loader based on version
 - Set as loader
 - Use data loader to get bytes, and dbc loader to read out the info.
