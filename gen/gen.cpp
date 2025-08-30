#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <regex>
#include <algorithm>
#include <sstream>

struct EntityAttribute {
    std::string name;
    std::string type;
    bool optional;
    bool list;
    bool set;
    std::string enumValues;
    
    EntityAttribute() : optional(false), list(false), set(false) {}
};

struct EntityDefinition {
    std::string name;
    std::string superType;
    std::vector<EntityAttribute> attributes;
    bool isAbstract;
    
    EntityDefinition() : isAbstract(false) {}
};

struct EnumDefinition {
    std::string name;
    std::vector<std::string> values;
};

struct TypeDefinition {
    std::string name;
    std::string baseType;
    bool isEnum;
    std::vector<std::string> enumValues;
    
    TypeDefinition() : isEnum(false) {}
};

class ExpressToCppGenerator {
private:
    std::map<std::string, EntityDefinition> entities;
    std::map<std::string, TypeDefinition> types;
    std::map<std::string, EnumDefinition> enums;
    std::set<std::string> builtInTypes;
    
public:
    ExpressToCppGenerator() {
        // Initialize built-in types
        builtInTypes.insert("INTEGER");
        builtInTypes.insert("REAL");
        builtInTypes.insert("NUMBER");
        builtInTypes.insert("STRING");
        builtInTypes.insert("BOOLEAN");
        builtInTypes.insert("LOGICAL");
        builtInTypes.insert("BINARY");
    }
    
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(' ');
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }
    
    std::string toUpperCase(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
    
    std::string toCppType(const std::string& expressType) {
        std::string type = trim(expressType);
        
        // Handle optional types
        bool isOptional = false;
        if (type.find("OPTIONAL") != std::string::npos) {
            isOptional = true;
            type = std::regex_replace(type, std::regex("OPTIONAL\\s+"), "");
        }
        
        // Handle list and set types
        bool isList = false, isSet = false;
        if (type.find("LIST") != std::string::npos) {
            isList = true;
            std::regex listRegex("LIST\\s*\\[.*?\\]\\s+OF\\s+(.+)");
            std::smatch match;
            if (std::regex_search(type, match, listRegex)) {
                type = match[1].str();
            }
        } else if (type.find("SET") != std::string::npos) {
            isSet = true;
            std::regex setRegex("SET\\s*\\[.*?\\]\\s+OF\\s+(.+)");
            std::smatch match;
            if (std::regex_search(type, match, setRegex)) {
                type = match[1].str();
            }
        }
        
        type = trim(type);
        
        // Convert basic types
        std::string cppType;
        if (type == "INTEGER") {
            cppType = "int";
        } else if (type == "REAL" || type == "NUMBER") {
            cppType = "double";
        } else if (type == "STRING") {
            cppType = "std::string";
        } else if (type == "BOOLEAN") {
            cppType = "bool";
        } else if (type == "LOGICAL") {
            cppType = "int"; // -1, 0, 1 for UNKNOWN, FALSE, TRUE
        } else if (type == "BINARY") {
            cppType = "std::vector<uint8_t>";
        } else {
            // User-defined type
            cppType = type;
        }
        
        // Wrap in containers if needed
        if (isList) {
            cppType = "std::vector<" + cppType + ">";
        } else if (isSet) {
            cppType = "std::set<" + cppType + ">";
        }
        
        if (isOptional) {
            cppType = "std::optional<" + cppType + ">";
        }
        
        return cppType;
    }
    
    void parseExpressFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        
        std::string line;
        std::string currentBlock;
        bool inEntity = false, inType = false;
        EntityDefinition currentEntity;
        TypeDefinition currentType;
        
        while (std::getline(file, line)) {
            line = trim(line);
            
            // Skip comments and empty lines
            if (line.empty() || line.substr(0, 2) == "--" || line.substr(0, 2) == "(*") {
                continue;
            }
            
            currentBlock += line + " ";
            
            // Check for complete statements
            if (line.back() == ';') {
                processStatement(trim(currentBlock));
                currentBlock.clear();
            }
        }
        
        file.close();
    }
    
    void processStatement(const std::string& statement) {
        // Entity definition
        std::regex entityRegex("ENTITY\\s+(\\w+)(?:\\s+SUBTYPE\\s+OF\\s*\\((\\w+)\\))?\\s*;?(.*)END_ENTITY", 
                              std::regex_constants::icase);
        std::smatch match;
        
        if (std::regex_search(statement, match, entityRegex)) {
            EntityDefinition entity;
            entity.name = match[1].str();
            if (match[2].matched) {
                entity.superType = match[2].str();
            }
            
            std::string attributesPart = match[3].str();
            parseEntityAttributes(entity, attributesPart);
            
            entities[entity.name] = entity;
            return;
        }
        
        // Type definition
        std::regex typeRegex("TYPE\\s+(\\w+)\\s*=\\s*(.+?)\\s*;?\\s*END_TYPE", 
                            std::regex_constants::icase);
        if (std::regex_search(statement, match, typeRegex)) {
            TypeDefinition type;
            type.name = match[1].str();
            std::string definition = match[2].str();
            
            // Check if it's an enumeration
            if (definition.find("ENUMERATION") != std::string::npos) {
                type.isEnum = true;
                parseEnumeration(type, definition);
            } else {
                type.baseType = definition;
            }
            
            types[type.name] = type;
            return;
        }
    }
    
    void parseEntityAttributes(EntityDefinition& entity, const std::string& attributesPart) {
        std::istringstream stream(attributesPart);
        std::string line;
        
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty()) continue;
            
            // Simple attribute parsing (can be enhanced for complex cases)
            std::regex attrRegex("(\\w+)\\s*:\\s*(.+?)(?:;|$)");
            std::smatch match;
            
            if (std::regex_search(line, match, attrRegex)) {
                EntityAttribute attr;
                attr.name = match[1].str();
                std::string typeStr = match[2].str();
                
                attr.optional = typeStr.find("OPTIONAL") != std::string::npos;
                attr.list = typeStr.find("LIST") != std::string::npos;
                attr.set = typeStr.find("SET") != std::string::npos;
                attr.type = toCppType(typeStr);
                
                entity.attributes.push_back(attr);
            }
        }
    }
    
    void parseEnumeration(TypeDefinition& type, const std::string& definition) {
        std::regex enumRegex("ENUMERATION\\s*OF\\s*\\(([^)]+)\\)");
        std::smatch match;
        
        if (std::regex_search(definition, match, enumRegex)) {
            std::string valuesList = match[1].str();
            std::istringstream stream(valuesList);
            std::string value;
            
            while (std::getline(stream, value, ',')) {
                value = trim(value);
                if (!value.empty()) {
                    type.enumValues.push_back(value);
                }
            }
        }
    }
    
    void generateCppStructs(const std::string& outputFile) {
        std::ofstream out(outputFile);
        if (!out.is_open()) {
            throw std::runtime_error("Cannot create output file: " + outputFile);
        }
        
        out << "#pragma once\n\n";
        out << "#include <string>\n";
        out << "#include <vector>\n";
        out << "#include <set>\n";
        out << "#include <optional>\n";
        out << "#include <memory>\n\n";
        out << "namespace IFC {\n\n";
        
        // Generate forward declarations
        out << "// Forward declarations\n";
        for (const auto& pair : entities) {
            out << "struct " << pair.first << ";\n";
        }
        out << "\n";
        
        // Generate enums
        for (const auto& pair : types) {
            if (pair.second.isEnum) {
                generateEnum(out, pair.second);
            }
        }
        
        // Generate type aliases
        out << "// Type aliases\n";
        for (const auto& pair : types) {
            if (!pair.second.isEnum) {
                out << "using " << pair.first << " = " << toCppType(pair.second.baseType) << ";\n";
            }
        }
        out << "\n";
        
        // Generate structs
        for (const auto& pair : entities) {
            generateStruct(out, pair.second);
        }
        
        out << "} // namespace IFC\n";
        out.close();
    }
    
    void generateEnum(std::ofstream& out, const TypeDefinition& type) {
        out << "enum class " << type.name << " {\n";
        for (size_t i = 0; i < type.enumValues.size(); ++i) {
            out << "    " << type.enumValues[i];
            if (i < type.enumValues.size() - 1) {
                out << ",";
            }
            out << "\n";
        }
        out << "};\n\n";
    }
    
    void generateStruct(std::ofstream& out, const EntityDefinition& entity) {
        out << "struct " << entity.name;
        if (!entity.superType.empty()) {
            out << " : public " << entity.superType;
        }
        out << " {\n";
        
        // Generate attributes
        for (const auto& attr : entity.attributes) {
            out << "    " << attr.type << " " << attr.name << ";\n";
        }
        
        // Generate constructor
        if (!entity.attributes.empty()) {
            out << "\n    " << entity.name << "() = default;\n";
            
            // Constructor with parameters
            out << "    " << entity.name << "(";
            for (size_t i = 0; i < entity.attributes.size(); ++i) {
                if (i > 0) out << ", ";
                out << "const " << entity.attributes[i].type << "& " << entity.attributes[i].name;
            }
            out << ")\n        : ";
            
            if (!entity.superType.empty()) {
                out << entity.superType << "()";
                if (!entity.attributes.empty()) out << ", ";
            }
            
            for (size_t i = 0; i < entity.attributes.size(); ++i) {
                if (i > 0) out << ", ";
                out << entity.attributes[i].name << "(" << entity.attributes[i].name << ")";
            }
            out << " {}\n";
        }
        
        out << "};\n\n";
    }
    
    void printStatistics() {
        std::cout << "Parsing completed:\n";
        std::cout << "- Entities: " << entities.size() << "\n";
        std::cout << "- Types: " << types.size() << "\n";
        std::cout << "- Enums: ";
        int enumCount = 0;
        for (const auto& pair : types) {
            if (pair.second.isEnum) enumCount++;
        }
        std::cout << enumCount << "\n";
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.exp> <output.hpp>\n";
        return 1;
    }
    
    try {
        ExpressToCppGenerator generator;
        
        std::cout << "Parsing EXPRESS file: " << argv[1] << "\n";
        generator.parseExpressFile(argv[1]);
        
        std::cout << "Generating C++ header: " << argv[2] << "\n";
        generator.generateCppStructs(argv[2]);
        
        generator.printStatistics();
        std::cout << "Generation completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}