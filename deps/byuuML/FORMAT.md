The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED",  "MAY", and "OPTIONAL" in this document are to be interpreted as described in [RFC 2119](https://www.ietf.org/rfc/rfc2119.txt).

Regular expressions given in this document are written surrounded by slashes, and follow the POSIX Extended Regular Expression syntax. They are compatible with nearly every regular expression syntax in wide use.

Literal strings given in this document are written surrounded by double quotes, and follow the C99 string literal syntax. They are compatible with nearly every string literal syntax in wide use.

# Disclaimer

This specification was obtained by reverse engineering the BML parser in the nall library, and does not necessarily reflect byuu's intentions in defining the format. However, strict compliance with this specification *will* ensure that a document can be parsed successfully by both `nall::BML` and `byuuML`.

# Structure

A byuuML document defines an ordered sequence of zero or more Tags.

A Tag consists of the following:

- A name. The name of a Tag may only contain Latin characters, Arabic numerals, hyphens, and periods, and must contain at least one. (i.e. a valid Tag name always matches `/^[-.A-Za-z0-9]+$/`)
- Data (optional). Data is free-form text content. There is no way for Data to contain a carriage return, but every other character (including line feed) is possible.
- Children. A Tag may optionally contain child Tags.

# Format

A byuuML document SHOULD have a file extension of `.bml`, and a MIME type of `text/prs.byuu-bml`.

A byuuML document SHOULD be encoded in a shift-free ASCII-compatible encoding, preferably UTF-8. This specification is defined in terms of characters, not bytes.

A byuuML document SHOULD NOT contain any NUL characters.

A document is split up into Lines. Every consecutive sequence of non-line-ending characters in the document is considered to be a Line. The line-ending characters are carriage return and line feed. "Empty" (zero-length) lines MUST be ignored. (In other words, the Lines consist of every match for `/[^\n\r]+/`)

If a line starts with two slashes (`/`), it is ignored. (Lines matching `/^\/\//` are ignored.)

## Indentation

A Line begins with indentation. Indentation consists of zero or more occurrences of the tab or space characters. The Indentation Level of a Line is defined as the number of tab or space characters that are found at the beginning of the line.

- An Indentation Level *greater than* the Indentation Level of the most recent Tag can denote one of two things:
    - If the first character after the indentation is a colon (`:`), the line is a Data Continuation for the most recent Tag. (See Data below.) Data Continuations MUST NOT be applicable to any Tag other than the *most* recent.
    - Otherwise, the line defines a *child* of the most recent Tag. The most recent Tag is now considered "open".
- An Indentation Level *equal* to the Indentation Level of the most recent Tag denotes a *sibling* of the Tag defined on the previous Line. (The two Tags share parents.)
- An Indentation Level *less than* the Indentation Level of the most recent Tag "closes" all open Tags with greater or equal Indentation Levels. This Line's Indentation Level MUST be exactly equal to the Indentation Level of some currently-"open" Tag. This line then defines a *sibling* of that Tag.

Tags with no parents (or, put another way, Tags whose parent is the document) MUST have an Indentation Level of zero.

If a byuuML document contains any line whose indentation does not meet these rules, the document is invalid.

## Tag

A Tag starts with a Tag Name, which is a sequence one or more of the following characters:

- Latin letters, capital and lowercase (`A`/`a` through `Z`/`z`)
- Arabic numerals (`0` through `9`)
- Hyphen (`-`)
- Period (`.`)

The Tag Name ends when the first character outside this set, or the end of the Line, is encountered.

(As mentioned above, Tag Names match `/^[-.A-Za-z0-9]+$/`)

## Data

- If the Tag Name is followed by an equals sign (`=`):
    - If the equals sign is followed by a double quote (`"`), the Data consists of every character up to (but not including) the next double quote. If the line ends without another double quote, the document is invalid. (There is no provision for escaping characters.) The remainder of the line (if any) is then parsed as Attributes.
    - Otherwise, the Data consists of every character up to (but not including) the next space character, or the end of the line, whichever occurs first. If the Data contains a double quote character, the document is invalid. The remainder of the line (if any) is then parsed as Attributes.
- If the Tag Name is followed by a colon (`:`), the Data consists of every remaining character on the Line. This includes leading whitespace, if any.
- If the Tag Name is followed by a space (` `), **or the end of the Line**, this Tag contains no Data. The remainder of the line (if any) is then parsed as Attributes.
- If the Tag Name is followed by any other character, including a slash (`/`), the document is invalid.

All forms of Data specification allow empty (zero-length) Data to be explicitly specified. Empty Data is **not** the same as **no** Data, as explained in the following section. However, parsers SHOULD expose no Data to applications the same way as empty Data. A separate "no Data" state SHOULD only be significant for Data Continuations.

Applications MAY wish to trim leading whitespace from Data, but this SHOULD NOT be done at the parser level.

### Data Continuation

A line that has a greater Indentation Level than the most recent Tag, and begins with a colon (`:`), is a Data Continuation line. If the Tag has Data, a line feed and the remaining characters on the line are appended to that Data. If the Tag has *no* Data, the remaining characters become that Tag's Data.

Note that *no* Data and *empty* Data are not the same. Consider the following examples:

    node
      : Some data
      : Some more data

The name of this Tag is `"node"`, and its Data is `"Some data\nSome more data"`.

    node:
      : Some data
      : Some more data

The name of this Tag is `"node"`, and its Data is `"\nSome data\nSome more data"`.

In spite of this, a parser MAY make no distinction between a Tag with empty Data and a Tag with no Data, so long as it parses the above cases correctly.

## Attributes

A Tag definition may be followed by Attributes. An Attribute starts with one or more **space** characters, followed by a Tag Name, followed by Data. A Tag with the given Name and (if present) Data is created as a child of that Tag.

NOTE: Tab characters are allowed as indentation, but not allowed at the beginning of attributes.

As an example:

    example foo=bar baz=bang bark dog: Adorable dog

Is mostly equivalent to the following:

    example
      foo=bar
      baz=bang
      bark
      dog: Adorable dog

Tags that are defined as Attributes do not become the "most recent" Tag and cannot have children. 

Anywhere an Attribute is expected, a pair of slashes (`/`) is a comment and ends processing of the line.

## BNF

This is standard BNF, except that `/.../` signifies a regular expression match.

             <document> ::= <lines>
    
                <lines> ::= <possible-line> <line-end> <lines> | <possible-line>
        <possible-line> ::= <line> | ""
             <line-end> ::= /[\r\n]/
    
                 <line> ::= <data-continuation> | <tag> | <comment>
    
    <data-continuation> ::= <indentation> ":" <remainder>
                 <tag> ::= <indentation> <tag-name> <tag-data>
    
          <indentation> ::= /[ \t]*/
            <tag-name> ::= /[-.A-Za-z0-9]+/
            <tag-data> ::= <tag-long-data> | <tag-short-data> | <attributes>
    
       <tag-long-data> ::= ":" <remainder>
      <tag-short-data> ::= "=" <attribute-data> <attributes>
      
       <attribute-data> ::= <quoted-data> | <unquoted-data>
          <quoted-data> ::= '"' /[^"]*/ '"'
        <unquoted-data> ::= /[^ ]*/
        
           <attributes> ::= <comment> | <attribute> <attributes> | <attribute>
            <attribute> ::= [ +] <tag-name> <tag-data>
    
            <remainder> ::= /[^\r\n]*/

## Example File

Included is an example file, [`test.bml`](test.bml). It attempts to check every edge case involving well-formed data, so you can establish that it's being parsed correctly. Make sure that you do **not** perform any line ending conversion before testing your parser on this file.

When parsed, the file should produce the following node hierarchy. (Note that, as stated at the beginning of this document, all strings are C-style strings.) <!-- If you are reading the raw Markdown as plain text, also note that there are two layers of escaping in effect: one for Markdown, one for the C compiler. -->

- `{"root-node", ""}`
    - `{"child-node-1", "datacont1"}`
    - `{"child-node-2", "\\ndatacont2"}`
- `{"root-node-2", " Data\\n   continues  \\napace\\n"}`
    - `{"child-node", ""}`
    - `{"child-node", ""}`
        - `{"attribute", "1 x=y"}`
        - `{"attribute", "2"}`
        - `{"attribute", "\_\x01\_\x02\_\x03\_\x04\_\x05\_\x06\_\x07\_\x08\_\x0b\_\x0c\_\x0e\_\x0f\_\x10\_\x11\_\x12\_\x13\_\t\_\x15\_\x16\_\x17\_\x18\_\x19\_\x1a\_\x1b\_\x1c\_\x1d\_\x1e\_\x1f\_\x7f\_"}`
        - `{"plus-attribute", " 127"}`
        - `{"additional-child", ""}`
            - `{"with-a-dataless-attribute", ""}`
            - `{"followed-by", "never escape\\"}`
            - `{"and", "\\\"never"}`
            - `{"give", ""}`
            - `{"up", ""}`
        - `{"another-child", ""}`
        - `{"yet-another-child", "//"}`
            - `{"without", ""}`
            - `{"comment", ""}`
    - `{"mixed-indentation-child", ""}`
    - `{"mixed-indentation-child-2", ""}`
    - `{"confusion", "maximum\nand then some"}`
        - `{"edge.case-1", "there\tis\tno\tedge.case-2:"}`
        - `{"after", ""}`
        - `{"all", ""}`
- `{"oh-no-more-root-node", ""}`
    - `{"a", "1"}`
    - `{"b", "2"}`
    - `{"c", "3\td=4"}`
- `{"lost-child", ""}`
    - `{"easily-misplaced", "very true"}`

Any deviation from this structure, including presence or absence of whitespace, indicates a non-compliant parser. Particular mishandled edge cases detected by `test.bml`, and likely causes for them:

- **Parsing MUST succeed...**  
  ...or either your parser is incorrect or the document is corrupted.
- **`root-node` MUST have two direct children...**  
  ...or either your parser does not ignore blank lines correctly, or it has another trivial parsing problem.
- **`child-node-1`'s Data MUST NOT begin with a newline...**  
  ...or your parser does not handle Data Continuations after no Data correctly.
- **`child-node-2`'s Data MUST begin with a newline...**  
  ...or your parser does not handle Data Continuations after empty Data correctly.
- **`root-node-2`'s Data MUST NOT contain either a colon or carriage return...**  
  ...or your parser does not treat carriage return as ending a line.
- **`root-node-2` MUST have exactly two direct children `child-node`...**  
  ...or your parser does not handle multiple explicit child Tags with the same Name correctly.
- **The second `child-node` MUST NOT have a child `{"x", "y"}` or `{"x", "y\\\""}`...**  
  ...or your parser does not handle quoted Attribute-style Data correctly.
- **The second `child-node` MUST have exactly three direct children `attribute`...**  
  ...or your parser does not handle multiple Attributes with the same Name properly.
- **The third `attribute`'s Data must be completely intact...**  
  ...or your parser probably discarded one or more control characters, and/or treated them as whitespace.
- **`plus-attribute` must begin with a single space character...**  
  ...or your parser is inappropriately trimming whitespace.
- **`additional-child` must have a child `with-a-dataless-attribute`...**  
  ...or your parser does not handle Attributes without Data.
- **`with-a-dataless-attribute` must not have any children...**  
  ...or your parser is probably incorrectly parenting Attributes meant for `additional-child` to its first Attribute.
- **`additional-child` must have children `and`, `give`, and `up`...**  
  ...or your parser has some form of escaping logic, which is forbidden.
- **`followed-by`'s data must end in exactly one backslash...**  
  ...or your parser has some form of escaping logic, which is forbidden.
- **`and`'s data must begin with exactly one backslash, then exactly one double quote...**  
  ...or your parser has some form of escaping logic, which is forbidden.
- **The second `child-node` must have a child `another-child`, which has no Data...**  
  ...or your parser was confused by the inline comment.
- **The second `child-node` must have a child `yet-another-child`, whose data is `"//"` and which has two children of its own...**  
  ...or your parser incorrectly detected an unquoted Data value of `"//"` as the beginning a comment. (Remember, comments are only allowed at the beginnings of lines and in locations where the Name of an *Attribute* is expected.)
- **`oh-no-more-root-node` must have exactly three children...**  
  ...or your parser incorrectly considers tab as a valid Attribute separator.
- **`lost-child` must be present...**  
  ...or your parser does not treat the end of the file as the end of a line.

If your parser handles *all* of these edge cases correctly, then it is probably compliant.

# Schema Best Practices

- byuuML documents SHOULD be understandable by humans.
- byuuML schemas SHOULD be permissive, ignoring data they do not understand.
- byuuML documents SHOULD contain data related to a common subject.
- byuuML documents SHOULD be as self-contained as possible.
- Root Tags SHOULD divide a document into logical groupings of data. (e.g. a Game Folder manifest might have a `board` Tag giving memory layout information, and an `information` Tag giving human-facing metadata.)
