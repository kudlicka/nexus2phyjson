#include <ncl/nxsmultiformat.h>
#include "json.hpp"

using json = nlohmann::json;

void processChildren(json& childrenJson, const NxsSimpleNode *parent) {
    for (auto child : parent->GetChildren()) {
        json childJson;
        if (child->GetTaxonIndex() < UINT_MAX) {
            childJson["taxon"] = child->GetTaxonIndex();
        }
        childJson["branch_length"] = child->GetEdgeToParent().GetDblEdgeLen();
        childJson["children"] = json::array();
        processChildren(childJson["children"], child);
        childrenJson += childJson;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " file.nex" << std::endl;
        return 1;
    }

    MultiFormatReader nexusReader(-1, NxsReader::IGNORE_WARNINGS);
    nexusReader.ReadFilepath(argv[1], MultiFormatReader::NEXUS_FORMAT);

    json phyjson;

    phyjson["format"] = "phyjson";
    phyjson["version"] = "1.0";
    phyjson["taxa"] = json::array();
    phyjson["trees"] = json::array();

    const unsigned numTaxaBlocks = nexusReader.GetNumTaxaBlocks();

    if (numTaxaBlocks > 1) {
        std::cerr << "Warning: Nexus2phyjson can only handle one taxa block, ignoring remaining blocks" << std::endl;
    }

    if (numTaxaBlocks > 0) {
        NxsTaxaBlock *taxaBlock = nexusReader.GetTaxaBlock(0);
        for (unsigned i = 0; i < taxaBlock->GetNTaxTotal(); i++) {
            phyjson["taxa"] += {{"id", i}, {"name", taxaBlock->GetTaxonLabel(i)}};
        }

        const unsigned numTreesBlocks = nexusReader.GetNumTreesBlocks(taxaBlock);
        if (numTreesBlocks > 1) {
            std::cerr << "Warning: Nexus2phyjson can only handle one trees block, ignoring remaining blocks" << std::endl;
        }
        if (numTreesBlocks > 0) {
            NxsTreesBlock *treesBlock = nexusReader.GetTreesBlock(taxaBlock, 0);
            for (unsigned i = 0; i < treesBlock->GetNumTrees(); i++) {
                const NxsFullTreeDescription &fullTreeDesc = treesBlock->GetFullTreeDescription(i);
                NxsSimpleTree tree(fullTreeDesc, -1, -1.0);

                json treeJson;
                treeJson["name"] = fullTreeDesc.GetName();
                treeJson["root"]["branch_length"] = tree.GetRootConst()->GetEdgeToParent().GetDblEdgeLen();
                treeJson["root"]["children"] = json::array();
                processChildren(treeJson["root"]["children"], tree.GetRootConst());
                phyjson["trees"] += treeJson;
            }
        }
    }

    std::cout << phyjson.dump(4) << std::endl;
}
