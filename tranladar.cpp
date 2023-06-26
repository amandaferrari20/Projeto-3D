#include <iostream>
#include <vector>
#include "tinyobjloader/tiny_obj_loader.h"


void transladarObjeto(std::vector<float>& positions, float translationZ) {
    for (size_t i = 0; i < positions.size(); i += 3) {
        positions[i + 2] += translationZ;
    }
}

int main() {
    std::string inputfile = "models/Car.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    tinyobj::attrib_t attrib;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        return 1;
    }

    // Transladar o objeto em -5 no eixo Z
    float translationZ = -5.0f;
    for (size_t s = 0; s < shapes.size(); ++s) {
        std::vector<float>& positions = attrib.vertices;
        transladarObjeto(positions, translationZ);
    }

    // Salvar o novo arquivo .obj
    std::string outputfile = "meu_objeto_translated.obj";
    ret = tinyobj::WriteObj(outputfile.c_str(), shapes, materials);

    if (ret) {
        std::cout << "Arquivo " << outputfile << " criado com sucesso!" << std::endl;
    } else {
        std::cerr << "Erro ao salvar o arquivo " << outputfile << std::endl;
    }

    return 0;
}

