I chose to use BML—the same markup language used by [higan](https://byuu.org/emulation/higan/)—for describing cartridge layouts in the [ARS](https://github.com/SolraBizna/ars-emu)'s Game Folder format. (I came to this decision, as well as the decision to use "Game Folders" in general, for mostly [the same reasons byuu did](https://byuu.org/emulation/higan/game-paks).)

Unfortunately, I couldn't find any technical documentation for BML. byuu has stated that there was documentation online at one point, but it wasn't very readable, and it has since been lost. While I could have made my own, similar markup language, if I were to do so I would be making a very bad problem worse—after all, there are already too many markup languages out there, and the worst thing one can possibly do in this situation is to make *another* one that is "similar" to an existing one.

A complete parser, under the GPLv3 license (I think), can be found in nall, which is a library of useful templates written by byuu and used in (among other things) higan. But I didn't want to change my project to use nall just so I could parse BML files.

So, I did two things:

- Reverse engineered the BML parser in nall to learn the format, writing [`FORMAT.md`](FORMAT.md) as I went
- Wrote this library, so I could parse BML without having to depend on nall

If you're already using nall, use its parser instead. If you're not, you've come to the right place.

Since there are already a number of other markup languages with the acronym BML, throughout this library I will be referring to the language as byuuML.

# License

This library is licensed under the zlib license (see [`LICENSE.md`](LICENSE.md)).

I don't claim to have originated or designed the byuuML format, or any copyright on it. I only claim to have written this library, and originated the sequences of words that comprise its documentation.

# Using the `byuuML` library

(If you are only interested in technical details about the byuuML file format, you want to read [`FORMAT.md`](FORMAT.md).)

This library is written in C++14. Bindings to other languages, including C, are not provided.

Add `byuuML.cc` to your project, add the directory containing `byuuML.hh` to your include path (if needed), and make sure your compiler is set to comply with C++14 or higher.

Files that must parse or process a byuuML document should `#include "byuuML.hh"`.

To parse a document, implement `byuuML::reader`. This class must implement a `read_more` function that reads some of the file and provides pointers to the data that was read. Ownership of the pointers belongs to the `byuuML::reader` instance, and pointers are not assumed to be valid between `read_more` calls.

Here is a simple example `byuuML::reader` that reads from a `std::ifstream` until EOF:

```c++
class ifreader : public byuuML::reader {
    char buf[512];
    std::ifstream& f;
public:
    ifreader(std::ifstream& f) : f(f) {}
    void read_more(const char*& begin, const char*& end) override {
        begin = buf;
        if(!f) {
            end = begin;
            return;
        }
        f.read(buf, sizeof(buf));
        end = begin + f.gcount();
    }
};
```

Pass an instance of your `reader` along to the constructor of `byuuML::document` and it will attempt to parse the file. If parsing fails, a `std::string` containing a human-readable error message is shown. (Most applications can ignore the message and indicate a generic failure.)

```c++
ifreader reader(std::cin);
byuuML::document document(reader);
```

If you need the `document` to have a longer lifetime, or need to pass it around various places, wrap it in a smart pointer such as `std::shared_ptr` or `std::unique_ptr`:

```c++
ifreader reader(std::cin);
std::shared_ptr<byuuML::document> document = std::make_shared<byuuML::document>(reader);
```

A `byuuML::document` is, conceptually, a tree of `byuuML::node`s. This tree is immutable and compact. A `document` acts as a simple container for the root `node`s, and each `node` acts as a container for its child `node`s. Because of the way `node`s are stored in memory, you also need to refer to a `document` in order to iterate on one. Here's an example:

```c++
// A simple pretty printer for byuuML nodes. Conveniently, it outputs a valid
// byuuML document, as long as there aren't any newlines in the Data.
void print_node(std::ostream& out, const byuuML::document& document,
                const byuuML::node& node, int indent_level = 0) {
    for(int n = 0; n < indent_level; ++n) {
        out << "    ";
    }
    out << node.name;
    if(!node.data.empty()) out << ":" << node.data;
    out << "\n";
    for(auto&& child : byuuML::node_in_document(node, document)) {
        print_node(out, document, child, indent_level + 1);
    }
}
void print_document(std::ostream& out,
                    const byuuML::document& document) {
    for(auto&& node : document) {
        print_node(out, document, node);
    }
}
```

Using iterators to do complicated parsing on a byuuML tree is a pain, if you're after specific information. Including `byuuML_query.hh` gives you a better option. The `query` method returns a `byuuML::cursor` object, providing a much less painful way to get specific data.

```c++
// The following two lines are equivalent
byuuML::cursor p = document.query("information", "title");
byuuML::cursor p = document.query("information").query("title");
```

`query` may be called on a `document`, `node_in_document`, or `cursor` object, and will descend the tree looking for the named node(s). `query` may also be called on a `node` if you explicitly pass a `document`:

```c++
byuuML::cursor p = node.query(document, "title");
```

If the node is not found, the `cursor` will evaluate to false:

```c++
byuuML::cursor p = document.query("information", "title");
if(p) std::cout << "A title was found!\n";
else std::cout << "A title was not found.\n";
```

You may call `get_node()` on a `cursor` to retrieve a reference to the found `node`, `name()` to retrieve its name, `data()` to retrieve its data, or `value<T>()` to retrieve its value in some type T. All four of these methods will throw `byuuML::missing_node_exception` if the node was not found, and `value<T>()` may throw an exception if converting the data to the desired type failed.

You may pass a default value to `data()` or `value<T>()`, and if the node was not found, the default will be used instead of throwing an exception. (`value<T>()` will still throw an exception if converting the data fails.)

Specializations of `value<T>()` are provided for `int`, `long`, `long long`, `unsigned long`, `unsigned long long`, `float`, `double`, and `long double`, and it will automatically work with any type `T` with a constructor `T(std::string)` or `T(const std::string&)`. (Note: `std::string` is such a type.)

```c++
// Print the CHR ROM size of a NES Game Pak. (If a size is not specified in the
// manifest.bml, the CHR ROM size is 0.)
std::cout << "CHR ROM size: "
          << document.query("board", "chr", "rom", "size").value<int>(0)
          << "\n";
```

If you're interested in more than one element of a type, you can do a range-based for on a `cursor`:

```c++
// Print the ROM mappings and masks designated by the manifest.bml of a SNES
// Game Pak.
for(auto&& c : document.query("board", "rom", "map")) {
    std::cout << "Address: " << c.query("address").data() << "\n";
    std::cout << "   Mask: " << c.query("mask").data() << "\n";
}
```

If you wish, you can use brackets instead of calling `query`:

```c++
// For every RAM chip in a a SNES Game Pak, print its size and whether it is
// persistent or volatile.
for(auto&& c : document["board"]["ram"]) {
    std::cout << "RAM chip: " << c["size"].value<int>() << " bytes (";
    if(c["volatile"])
        std::cout << "volatile";
    else
        std::cout << "persistent";
    std::cout << ")\n";
}
```

(The exception is `node`, since it would need a corresponding `document`. You can, however, make a `node_in_document` and use brackets on that.)
