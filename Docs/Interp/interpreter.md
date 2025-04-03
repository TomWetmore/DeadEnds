### Swift Version of the DeadEnds Interpreter



Overall process.

A C program parses a DeadEnds program (*aka* script) into PNodes,transforms those PNodes into SExprs (S-Expressions), and then writes the SExprs to a text file.

A Swift DeadEnds program reads the SExpr text file and converts the SExprs to PNodes. The Swift form of PNodes varies a bit from the C version. For example the C version links together (via next pointers) the sequences of PNodes that make up 'blocks' (e.g., function and procedure bodies, loop bodies), while the Swift version adds a new PNode type (.block) whose value is an array of PNodes. The Swift version also takes advantage of Swift's powerful enum feature that lets each enum value to have associated types. The Swift definiton of PNode (in `PNode.swift`) is simpler and easier to understand than C's (in `pnode.h`) 

What does Swift DeadEnds have to do to run a DeadEnds program:

It must read the S-Expressions, and as it reads them, it must find the procedure definitions and add them to a procedure table, find the function definitions and add them to a function table, and find all the global variable definitions and add them to the global symbol table.

It must build the context in which the program will rull. That context includes the three tables just mentioned and the database with the genealogical information.

There is a Program class that currently has the four items mention above as properties.



Some of the Data Types Involved



There is a data type, Program, with three properties:

```swift
public class Program {
  var procedureTable: [String: PNode]
  var functionTable: [String: PNode]
  var globalSymbolTable: [String: PValue]
```

The three tables hold information extracted from the S-Expression. the initializer for Program is:

```swift
init(globalSymbolTable: SymbolTable: [String: PValue] = [:],
        procedureTable: [String: PNode] = [:], functionTable: [String: PNode] = [:]) {
    self.globalSymbolTable = globalSymbolTable
    self.procedureTable = procedureTable
    self.functionTable = functionTable
}
```

Programs are currently created by a factory method on the Program class (maybe it should be a stand alone function). Here is its code:

```swift
static func loadFromSExpr(_ sexprs: [SExpr]) throws -> Program {
    var procTable: [String: PNode] = [:]
    var funcTable: [String: PNode] = [:]
    var globalTable: SymbolTable = [:]
    for sexpr in sexprs {
        switch sexpr {
            case .list(let elements):
            guard let first = elements.first else {
                throw RuntimeError.invalidSyntax("Empty list encountered")
            }
            switch first {
                case .atom("proc"):
                let procDef = try sexpr.toPNode()
                if case let PNodeValue.procedureDef(name, _, _) = procDef.value {
                    procTable[name] = procDef
                } else {
                    throw RuntimeError.invalidSyntax("Invalid procedure definition")
                }
                case .atom("func"):
                let funcDef = try sexpr.toPNode()
                if case let PNodeValue.functionDef(name, _, _) = funcDef.value {
                    funcTable[name] = funcDef
                } else {
                    throw RuntimeError.invalidSyntax("Invalid function definition")
                }
                case .atom("global"):
                let (name, value) = try parseVariableDeclaration(sexpr)
                globalTable[name] = value
                default:
                throw RuntimeError.invalidSyntax("Unexpected top-level expression: \(first)")
            }
            default:
            throw RuntimeError.invalidSyntax("Top-level expressions must be lists")
        }
    }
    return Program(globalSymbolTable: globalTable, procedureTable: procTable,
                   functionTable: funcTable)
}
```

  

loadFromSExpr creates a Program. Note that it uses the method SExpr.toPNode to do the actual processing of the S-Expressions that that creates the PNodes.    

