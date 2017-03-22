My playground for the examples of the famous book: ["Modern C++ Design"](http://erdani.com/index.php/books/modern-c-design/)

# Build
[![Build Status](https://travis-ci.org/LukasWoodtli/ModernCppDesign.svg?branch=master)](https://travis-ci.org/LukasWoodtli/ModernCppDesign)

# Contents

## Part I: Techniques

### Chapter 1. Policy-Based Class Design

| Section | Title                                   | Page | Done |
|---------|-----------------------------------------|------|------|
| 1.1     | The Multiplicity of Software Design     |    2 |   ✓  |
| 1.2     | The Failure of the Do-It-All Interface  |    3 |   ✓  |
| 1.3     | Multiple Inheritance to the Rescue?     |    4 |   ✓  |
| 1.4     | The Benefit of Templates                |    5 |   ✓  |
| 1.5     | Policies and Policy Classes             |    6 |   ✓  |
| 1.6     | Enriched Policies                       |    9 |   ✓  |
| 1.7     | Destructors of Policy Classes           |   10 |   ✓  |
| 1.8     | Optional Functionality Through Incomplete Instantiation | 11 |  |
| 1.9     | Combining Policy Classes                |   12 |      |
| 1.10    | Customizing Structure with Policy Classes | 13 |      |
| 1.11    | Compatible and Incompatible Policies    |   14 |      |
| 1.12    | Decomposing a Class into Policies       |   16 |      |
| 1.13    | Summary                                 |   17 |      |


### Chapter 2. Techniques

| Section | Title                               | Page | Done |
|---------|-------------------------------------|------|------|
|  2.1    | Compile-Time Assertions             |   19 |      |
|  2.2    | Partial Template Specialization     |   22 |      |
|  2.3    | Local Classes                       |   23 |      |
|  2.4    | Mapping Integral Constants to Types |   24 |      |
|  2.5    | Type-to-Type Mapping                |   26 |      |
|  2.6    | Type Selection                      |   28 |      |
|  2.7    | Detecting Convertibility and Inheritance at Compile Time | 29 | |
|  2.8    | A Wrapper Around type_info          |   32 |      |
|  2.9    | NullType and EmptyType              |   34 |      |
| 2.10    | Type Traits                         |   34 |      |
| 2.11    | Summary                             |   40 |      |


### Chapter 3. Typelists

| Section | Title | Page | Done |
|-----------|-----------|-----------|-----------|
| 3.1	| The Need for Typelists	| 42 |	|
| 3.2	| Defining Typelists	| 43 |	|
| 3.3	| Linearizing Typelist Creation	| 45 |	|
| 3.4	| Calculating Length	| 45 |	|
| 3.5	| Intermezzo	| 46 |	|
| 3.6	| Indexed Access	| 47 |	|
| 3.7	| Searching Typelists	| 48 |	|
| 3.8	| Appending to Typelists	| 49 |	|
| 3.9	| Erasing a Type from a Typelist	| 50 |	|
| 3.10	| Erasing Duplicates	| 51 |	|
| 3.11	| Replacing an Element in a Typelist	| 52 |	|
| 3.12	| Partially Ordering Typelists	| 53 |	|
| 3.13	| Class Generation with Typelists	| 56 |	|
| 3.14	| Summary	| 65 |	|
| 3.15	| Typelist Quick Facts	| 66 |	|


### Chapter 4. Small-Object Allocation

| Section | Title | Page | Done |
|-----------|-----------|-----------|-----------|
| 4.1	| The Default Free Store Allocator	| 68 |	|
| 4.2	| The Workings of a Memory Allocator	| 69 |	|
| 4.3	| A Small-Object Allocator	| 70 |	|
| 4.4	| Chunks	| 71 |	|
| 4.5	| The Fixed-Size Allocator	| 74 |	|
| 4.6	| The SmallObjAllocator Class	| 77 |	|
| 4.7	| A Hat Trick	| 79 |	|
| 4.8	| Simple, Complicated, Yet Simple in the End	| 81 |	|
| 4.9	| Administrivia	| 82 |	|
| 4.10	| Summary	| 83 |	|
| 4.11	| Small-Object Allocator Quick Facts	| 83 |	|

## Part II: Components

### Chapter 5. Generalized Functors

| Section | Title | Page | Done |
|-----------|-----------|-----------|-----------|
| 5.1	| The Command Design Pattern	| 86 |	|
| 5.2	| Command in the Real World	| 89 |	|
| 5.3	| C++ Callable Entities	| 89 |	|
| 5.4	| The Functor Class Template Skeleton	| 91 |	|
| 5.5	| Implementing the Forwarding Functor::operator()	| 95 |	|
| 5.6	| Handling Functors	| 96 |	|
| 5.7	| Build One, Get One Free	| 98 |	|
| 5.8	| Argument and Return Type Conversions	| 99 |	|
| 5.9	| Handling Pointers to Member Functions	| 101 |	|
| 5.10	| Binding	| 104 |	|
| 5.11	| Chaining Requests	| 106 |	|
| 5.12	| Real-World Issues I: The Cost of Forwarding Functions	| 107 |	|
| 5.13	| Real-World Issues II: Heap Allocation	| 108 |	|
| 5.14	| Implementing Undo and Redo with Functor	| 110 |	|
| 5.15	| Summary	| 110 |	|
| 5.16	| Functor Quick Facts	| 111 |	|


### Chapter 6. Implementing Singletons

| Section | Title | Page | Done |
|-----------|-----------|-----------|-----------|
| 6.1	| Static Data + Static Functions != Singleton	| 113 |	|
| 6.2	| The Basic C++ Idioms Supporting Singletons	| 114 |	|
| 6.3	| Enforcing the Singleton's Uniqueness	| 116 |	|
| 6.4	| Destroying the Singleton	| 116 |	|
| 6.5	| The Dead Reference Problem	| 118 |	|
| 6.6	| Addressing the Dead Reference Problem (I): The Phoenix Singleton	| 120 |	|
| 6.7	| Addressing the Dead Reference Problem (II): Singletons with Longevity	| 122 |	|
| 6.8	| Implementing Singletons with Longevity	| 125 |	|
| 6.9	| Living in a Multithreaded World	| 128 |	|
| 6.10	| Putting It All Together	| 130 |	|
| 6.11	| Working with SingletonHolder	| 134 |	|
| 6.12	| Summary	| 136 |	|
| 6.13	| SingletonHolder Class Template Quick Facts	| 136 |	|


### Chapter 7. Smart Pointers

| Section | Title | Page | Done |
|-----------|-----------|-----------|-----------|
| 7.1	| Smart Pointers 101	| 138 |	|
| 7.2	| The Deal	| 139 |	|
| 7.3	| Storage of Smart Pointers	| 140 |	|
| 7.4	| Smart Pointer Member Functions	| 142 |	|
| 7.5	| Ownership-Handling Strategies	| 143 |	|
| 7.6	| The Address-of Operator	| 150 |	|
| 7.7	| Implicit Conversion to Raw Pointer Types	| 151 |	|
| 7.8	| Equality and Inequality	| 153 |	|
| 7.9	| Ordering Comparisons	| 157 | |
| 7.10	| Checking and Error Reporting	| 159 |	|
| 7.11	| Smart Pointers to const and const Smart Pointers	| 161 |	|
| 7.12	| Arrays	| 161 |	|
| 7.13	| Smart Pointers and Multithreading	| 162 |	|
| 7.14	| Putting It All Together	| 165 |	|
| 7.15	| Summary	| 171 |	|
| 7.16	| SmartPtr Quick Facts	| 171 |	|


### Chapter 8. Object Factories

| Section | Title | Page | Done |
|-----------|-----------|-----------|-----------|
| 8.1	| The Need for Object Factories	| 174 |	|
| 8.2	| Object Factories in C++: Classes and Objects	| 175 |	|
| 8.3	| Implementing an Object Factory	| 176 |	|
| 8.4	| Type Identifiers	| 180 |	|
| 8.5	| Generalization	| 181 |	|
| 8.6	| Minutiae	| 184 |	|
| 8.7	| Clone Factories	| 185 |	|
| 8.8	| Using Object Factories with Other Generic Components	| 188 |	|
| 8.9	| Summary	| 189 |	|
| 8.10	| Factory Class Template Quick Facts	| 189 |	|
| 8.11	| CloneFactory Class Template Quick Facts	| 190 |	|


### Chapter 9. Abstract Factory

| Section | Title | Page | Done |
|-----------|-----------|-----------|-----------|
| 9.1	| The Architectural Role of Abstract Factory	| 191 |	|
| 9.2	| A Generic Abstract Factory Interface	| 193 |	|
| 9.3	| Implementing AbstractFactory	| 196 |	|
| 9.4	| A Prototype-Based Abstract Factory Implementation	| 199 |	|
| 9.5	| Summary	| 202 |	|
| 9.6	| AbstractFactory and ConcreteFactory Quick Facts	| 203 |	|


### Chapter 10. Visitor

| Section | Title | Page | Done |
|-----------|-----------|-----------|-----------|
| 10.1	| Visitor Basics	| 205 |	|
| 10.2	| Overloading and the Catch-All Function	| 210 |	|
| 10.3	| An Implementation Refinement: The Acyclic Visitor	| 211 |	|
| 10.4	| A Generic Implementation of Visitor	| 215 |	|
| 10.5	| Back to the "Cyclic" Visitor	| 221 |	|
| 10.6	| Hooking Variations	| 223 |	|
| 10.7	| Summary	| 226 |	|
| 10.8	| Visitor Generic Components Quick Facts	| 226 |	|


### Chapter 11. Multimethods

| Section | Title | Page | Done |
|-----------|-----------|-----------|-----------|
| 11.1	| What Are Multimethods?	| 228 |	|
| 11.2	| When Are Multimethods Needed?	| 229 |	|
| 11.3	| Double Switch-on-Type: Brute Force	| 230 |	|
| 11.4	| The Brute-Force Approach Automated	| 232 |	|
| 11.5	| Symmetry with the Brute-Force Dispatcher	| 237 |	|
| 11.6	| The Logarithmic Double Dispatcher	| 240 |	|
| 11.7	| FnDispatcher and Symmetry	| 245 |	|
| 11.8	| Double Dispatch to Functors	| 246 |	|
| 11.9	| Converting Arguments: static_cast or dynamic_cast?	| 248 |	|
| 11.10	| Constant-Time Multimethods: Raw Speed	| 252 |	|
| 11.11	| BasicDispatcher and BasicFastDispatcher as Policies	| 255 |	|
| 11.12	| Looking Forward	| 257 |	|
| 11.13	| Summary	| 258 |	|
| 11.14	| Double Dispatcher Quick Facts	| 259 |	|
