#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>

// Function to convert a GUID to a string
std::string guidToString(uint8_t guid[16]) {
    std::ostringstream ss;
    for (int i = 0; i < 16; ++i) {
        if (i == 4 || i == 6 || i == 8 || i == 10)
            ss << '-';
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(guid[i]);
    }
    return ss.str();
}

class Uasset {
public:
    struct Header {
        uint32_t EPackageFileTag;
        int32_t LegacyFileVersion;
        int32_t LegacyUE3Version;
        int32_t FileVersionUE4;
        int32_t FileVersionUE5;
        int32_t FileVersionLicenseeUE4;
        std::vector<std::pair<std::string, int32_t>> CustomVersions;
        int32_t TotalHeaderSize;
        std::string FolderName;
        uint32_t PackageFlags;
        int32_t NameCount;
        int32_t NameOffset;
        uint32_t SoftObjectPathsCount;
        uint32_t SoftObjectPathsOffset;
        std::string LocalizationId;
        int32_t GatherableTextDataCount;
        int32_t GatherableTextDataOffset;
        int32_t ExportCount;
        int32_t ExportOffset;
        int32_t ImportCount;
        int32_t ImportOffset;
        int32_t DependsOffset;
        uint32_t SoftPackageReferencesCount;
        uint32_t SoftPackageReferencesOffset;
        int32_t SearchableNamesOffset;
        int32_t ThumbnailTableOffset;
        std::string Guid;
        std::string PersistentGuid;
        std::string OwnerPersistentGuid;
        std::vector<std::pair<int32_t, int32_t>> Generations;
        std::string SavedByEngineVersion;
        std::string CompatibleWithEngineVersion;
        uint32_t CompressionFlags;
        uint32_t PackageSource;
        uint32_t AdditionalPackagesToCookCount;
        int32_t NumTextureAllocations;
        int32_t AssetRegistryDataOffset;
        int64_t BulkDataStartOffset;
        int32_t WorldTileInfoDataOffset;
        std::vector<int32_t> ChunkIDs;
        int32_t ChunkID;
        int32_t PreloadDependencyCount;
        int32_t PreloadDependencyOffset;
        int32_t NamesReferencedFromExportDataCount;
        int64_t PayloadTocOffset;
        int32_t DataResourceOffset;
        int32_t EngineChangelist; // Add this field
    };

    struct Import {
        std::string classPackage;
        std::string className;
        int32_t outerIndex;
        std::string objectName;
        std::string packageName;
        int32_t bImportOptional;
    };

    struct Export {
        int32_t classIndex;
        int32_t superIndex;
        int32_t templateIndex;
        int32_t outerIndex;
        std::string objectName;
        uint32_t objectFlags;
        int64_t serialSize;
        int64_t serialOffset;
        int32_t bForcedExport;
        int32_t bNotForClient;
        int32_t bNotForServer;
        std::string packageGuid;
        uint32_t packageFlags;
        int32_t bNotAlwaysLoadedForEditorGame;
        int32_t bIsAsset;
        int32_t bGeneratePublicHash;
        int32_t firstExportDependency;
        int32_t serializationBeforeSerializationDependencies;
        int32_t createBeforeSerializationDependencies;
        int32_t serializationBeforeCreateDependencies;
        int32_t createBeforeCreateDependencies;
        std::vector<std::string> data;
    };

    struct Name {
        std::string Name;
        uint16_t NonCasePreservingHash;
        uint16_t CasePreservingHash;
    };

    Header header;
    std::vector<Name> names;
    std::vector<Import> imports;
    std::vector<Export> exports;

    Uasset() = default;

    bool parse(const std::vector<uint8_t>& bytes);
private:
    size_t currentIdx = 0;
    const std::vector<uint8_t>* bytesPtr = nullptr;

    uint16_t readUint16();
    int32_t readInt32();
    uint32_t readUint32();
    int64_t readInt64();
    uint64_t readUint64();
    std::string readFString();
    std::string readGuid();
};

bool Uasset::parse(const std::vector<uint8_t>& bytes) {
    currentIdx = 0;
    bytesPtr = &bytes;

    // Reading the header
    header.EPackageFileTag = readUint32();
    std::cout << "EPackageFileTag: " << header.EPackageFileTag << std::endl;

    header.LegacyFileVersion = readInt32();
    std::cout << "LegacyFileVersion: " << header.LegacyFileVersion << std::endl;

    header.LegacyUE3Version = readInt32();
    std::cout << "LegacyUE3Version: " << header.LegacyUE3Version << std::endl;

    header.FileVersionUE4 = readInt32();
    std::cout << "FileVersionUE4: " << header.FileVersionUE4 << std::endl;

    if (header.LegacyFileVersion <= -8) {
        header.FileVersionUE5 = readInt32();
        std::cout << "FileVersionUE5: " << header.FileVersionUE5 << std::endl;
    }

    header.FileVersionLicenseeUE4 = readInt32();
    std::cout << "FileVersionLicenseeUE4: " << header.FileVersionLicenseeUE4 << std::endl;

    // CustomVersions
    int32_t customVersionsCount = readInt32();
    std::cout << "CustomVersions Count: " << customVersionsCount << std::endl;
    for (int32_t i = 0; i < customVersionsCount; ++i) {
        std::string key = readGuid();
        int32_t version = readInt32();
        header.CustomVersions.push_back({ key, version });
        std::cout << "CustomVersion[" << i << "]: " << key << " - " << version << std::endl;
    }

    header.TotalHeaderSize = readInt32();
    std::cout << "TotalHeaderSize: " << header.TotalHeaderSize << std::endl;

    header.FolderName = readFString();
    std::cout << "FolderName: " << header.FolderName << std::endl;

    header.PackageFlags = readUint32();
    std::cout << "PackageFlags: " << header.PackageFlags << std::endl;

    header.NameCount = readInt32();
    std::cout << "NameCount: " << header.NameCount << std::endl;

    header.NameOffset = readInt32();
    std::cout << "NameOffset: " << header.NameOffset << std::endl;

    if (header.FileVersionUE5 >= 0x0151) { // VER_UE5_ADD_SOFTOBJECTPATH_LIST
        header.SoftObjectPathsCount = readUint32();
        header.SoftObjectPathsOffset = readUint32();
    }

//    if (header.FileVersionUE4 >= 0x0D19) { // VER_UE4_ADDED_PACKAGE_SUMMARY_LOCALIZATION_ID
        header.LocalizationId = readFString();
//    }

    if (header.FileVersionUE4 >= 0x0E14) { // VER_UE4_SERIALIZE_TEXT_IN_PACKAGES
        header.GatherableTextDataCount = readInt32();
        header.GatherableTextDataOffset = readInt32();
    }

    header.ExportCount = readInt32();
    header.ExportOffset = readInt32();
    header.ImportCount = readInt32();
    header.ImportOffset = readInt32();
    header.DependsOffset = readInt32();

    if (header.FileVersionUE4 >= 0x0154) { // VER_UE4_ADD_STRING_ASSET_REFERENCES_MAP
        header.SoftPackageReferencesCount = readInt32();
        header.SoftPackageReferencesOffset = readInt32();
    }

    if (header.FileVersionUE4 >= 0x0163) { // VER_UE4_ADDED_SEARCHABLE_NAMES
        header.SearchableNamesOffset = readInt32();
    }

    header.ThumbnailTableOffset = readInt32();
    header.Guid = readGuid();

    if (header.FileVersionUE4 >= 0x0166) { // VER_UE4_ADDED_PACKAGE_OWNER
        header.PersistentGuid = readGuid();
    }

    if (header.FileVersionUE4 >= 0x0166 && header.FileVersionUE4 < 0x0183) { // VER_UE4_NON_OUTER_PACKAGE_IMPORT
        header.OwnerPersistentGuid = readGuid();
    }

    int32_t generationsCount = readInt32();
    header.Generations.clear();
    for (int32_t i = 0; i < generationsCount; ++i) {
        int32_t exportCount = readInt32();
        int32_t nameCount = readInt32();
        header.Generations.push_back({ exportCount, nameCount });
    }

    if (header.FileVersionUE4 >= 0x0171) { // VER_UE4_ENGINE_VERSION_OBJECT
        header.SavedByEngineVersion = std::to_string(readUint16()) + "." +
            std::to_string(readUint16()) + "." +
            std::to_string(readUint16()) + "-" +
            std::to_string(readUint32()) + "+" +
            readFString();
    }
    else {
        header.EngineChangelist = readInt32();
    }

    if (header.FileVersionUE4 >= 0x0175) { // VER_UE4_PACKAGE_SUMMARY_HAS_COMPATIBLE_ENGINE_VERSION
        header.CompatibleWithEngineVersion = std::to_string(readUint16()) + "." +
            std::to_string(readUint16()) + "." +
            std::to_string(readUint16()) + "-" +
            std::to_string(readUint32()) + "+" +
            readFString();
    }
    else {
        header.CompatibleWithEngineVersion = header.SavedByEngineVersion;
    }

    header.CompressionFlags = readUint32();

    int32_t compressedChunksCount = readInt32();
    if (compressedChunksCount > 0) {
        std::cerr << "Asset compressed" << std::endl;
        return false;
    }

    header.PackageSource = readUint32();
    header.AdditionalPackagesToCookCount = readUint32();
    if (header.AdditionalPackagesToCookCount > 0) {
        std::cerr << "AdditionalPackagesToCook has items" << std::endl;
        return false;
    }

    if (header.LegacyFileVersion > -7) {
        header.NumTextureAllocations = readInt32();
    }

    header.AssetRegistryDataOffset = readInt32();
    header.BulkDataStartOffset = readInt64();

    if (header.FileVersionUE4 >= 0x0183) { // VER_UE4_WORLD_LEVEL_INFO
        header.WorldTileInfoDataOffset = readInt32();
    }

    if (header.FileVersionUE4 >= 0x0191) { // VER_UE4_CHANGED_CHUNKID_TO_BE_AN_ARRAY_OF_CHUNKIDS
        int32_t chunkIDsCount = readInt32();
        header.ChunkIDs.clear();
        if (chunkIDsCount > 0) {
            for (int32_t i = 0; i < chunkIDsCount; ++i) {
                header.ChunkIDs.push_back(readInt32());
            }
        }
        else {
            std::cerr << "ChunkIDs has items" << std::endl;
            return false;
        }
    }
    else if (header.FileVersionUE4 >= 0x0192) { // VER_UE4_ADDED_CHUNKID_TO_ASSETDATA_AND_UPACKAGE
        header.ChunkID = readInt32();
    }

    if (header.FileVersionUE4 >= 0x0194) { // VER_UE4_PRELOAD_DEPENDENCIES_IN_COOKED_EXPORTS
        header.PreloadDependencyCount = readInt32();
        header.PreloadDependencyOffset = readInt32();
    }
    else {
        header.PreloadDependencyCount = -1;
        header.PreloadDependencyOffset = 0;
    }

    if (header.FileVersionUE5 >= 0x0196) { // VER_UE5_NAMES_REFERENCED_FROM_EXPORT_DATA
        header.NamesReferencedFromExportDataCount = readInt32();
    }

    if (header.FileVersionUE5 >= 0x0197) { // VER_UE5_PAYLOAD_TOC
        header.PayloadTocOffset = readInt64();
    }
    else {
        header.PayloadTocOffset = -1;
    }

    if (header.FileVersionUE5 >= 0x0198) { // VER_UE5_DATA_RESOURCES
        header.DataResourceOffset = readInt32();
    }

    // Reading names
    currentIdx = header.NameOffset;
    for (int32_t i = 0; i < header.NameCount; ++i) {
        Name name;
        name.Name = readFString();
        name.NonCasePreservingHash = readUint16();
        name.CasePreservingHash = readUint16();
        names.push_back(name);
    }

    // Reading imports
    currentIdx = header.ImportOffset;
    for (int32_t i = 0; i < header.ImportCount; ++i) {
        Import import;
import.classPackage = readFString();
import.className = readFString();
import.outerIndex = readInt32();
import.objectName = readFString();
import.packageName = readFString();
import.bImportOptional = readInt32();
        imports.push_back(import);
    }

    // Reading exports
    currentIdx = header.ExportOffset;
    for (int32_t i = 0; i < header.ExportCount; ++i) {
        Export exportData;
        exportData.classIndex = readInt32();
        exportData.superIndex = readInt32();
        exportData.templateIndex = readInt32();
        exportData.outerIndex = readInt32();
        exportData.objectName = readFString();
        exportData.objectFlags = readUint32();
        exportData.serialSize = readInt64();
        exportData.serialOffset = readInt64();
        exportData.bForcedExport = readInt32();
        exportData.bNotForClient = readInt32();
        exportData.bNotForServer = readInt32();
        exportData.packageGuid = readFString();
        exportData.packageFlags = readUint32();
        exportData.bNotAlwaysLoadedForEditorGame = readInt32();
        exportData.bIsAsset = readInt32();
        exportData.bGeneratePublicHash = readInt32();
        exportData.firstExportDependency = readInt32();
        exportData.serializationBeforeSerializationDependencies = readInt32();
        exportData.createBeforeSerializationDependencies = readInt32();
        exportData.serializationBeforeCreateDependencies = readInt32();
        exportData.createBeforeCreateDependencies = readInt32();

        // Reading export data
        if (exportData.serialSize > 0) {
            currentIdx = exportData.serialOffset;
            std::string nodeNameRef = readFString();
            exportData.data.push_back(nodeNameRef);
            uint32_t flags = readUint32();
            exportData.data.push_back(std::to_string(flags));
        }
        exports.push_back(exportData);
    }

    return true;
}

uint16_t Uasset::readUint16() {
    if (currentIdx + sizeof(uint16_t) > bytesPtr->size()) {
        std::cerr << "Out of bounds read (uint16)" << std::endl;
        std::terminate(); // or handle the error appropriately
    }
    uint16_t val;
    std::memcpy(&val, &(*bytesPtr)[currentIdx], sizeof(val));
    currentIdx += sizeof(val);
    return val;
}

int32_t Uasset::readInt32() {
    if (currentIdx + sizeof(int32_t) > bytesPtr->size()) {
        std::cerr << "Out of bounds read (int32)" << std::endl;
        std::terminate(); // or handle the error appropriately
    }
    int32_t val;
    std::memcpy(&val, &(*bytesPtr)[currentIdx], sizeof(val));
    currentIdx += sizeof(val);
    return val;
}

uint32_t Uasset::readUint32() {
    if (currentIdx + sizeof(uint32_t) > bytesPtr->size()) {
        std::cerr << "Out of bounds read (uint32)" << std::endl;
        std::terminate(); // or handle the error appropriately
    }
    uint32_t val;
    std::memcpy(&val, &(*bytesPtr)[currentIdx], sizeof(val));
    currentIdx += sizeof(val);
    return val;
}

int64_t Uasset::readInt64() {
    if (currentIdx + sizeof(int64_t) > bytesPtr->size()) {
        std::cerr << "Out of bounds read (int64)" << std::endl;
        std::terminate(); // or handle the error appropriately
    }
    int64_t val;
    std::memcpy(&val, &(*bytesPtr)[currentIdx], sizeof(val));
    currentIdx += sizeof(val);
    return val;
}

uint64_t Uasset::readUint64() {
    if (currentIdx + sizeof(uint64_t) > bytesPtr->size()) {
        std::cerr << "Out of bounds read (uint64)" << std::endl;
        std::terminate(); // or handle the error appropriately
    }
    uint64_t val;
    std::memcpy(&val, &(*bytesPtr)[currentIdx], sizeof(val));
    currentIdx += sizeof(val);
    return val;
}

std::string Uasset::readFString() {
    int32_t length = readInt32();
    if (length == 0) return "";
    if (length > 0) {
        if (currentIdx + length > bytesPtr->size()) {
            std::cerr << "Out of bounds read (FString)" << std::endl;
            std::terminate(); // or handle the error appropriately;
        }
        std::string str((*bytesPtr).begin() + currentIdx, (*bytesPtr).begin() + currentIdx + length - 1);
        currentIdx += length;
        return str;
    }
    else {
        length = -length * 2;
        if (currentIdx + length > bytesPtr->size()) {
            std::cerr << "Out of bounds read (FString)" << std::endl;
            std::terminate(); // or handle the error appropriately;
        }
        std::wstring wstr((wchar_t*)(&(*bytesPtr)[currentIdx]), length / 2 - 1);
        currentIdx += length;
        std::string result;
        result.reserve(wstr.size());
        for (wchar_t wc : wstr) {
            result.push_back(static_cast<char>(wc));
        }
        return result;
    }
}

std::string Uasset::readGuid() {
    uint8_t guid[16];
    if (currentIdx + sizeof(guid) > bytesPtr->size()) {
        std::cerr << "Out of bounds read (Guid)" << std::endl;
        std::terminate(); // or handle the error appropriately
    }
    std::memcpy(guid, &(*bytesPtr)[currentIdx], sizeof(guid));
    currentIdx += sizeof(guid);
    return guidToString(guid);
}

int main() {
    std::ifstream file("C:/Users/kapis/Downloads/Blueprint/BP_FrontEndPlayerController.uasset", std::ios::binary);

    if (!file) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }
    std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    Uasset uasset;
    if (!uasset.parse(bytes)) {
        std::cerr << "Failed to parse uasset file" << std::endl;
        return 1;
    }

    // Access parsed data
    std::cout << "Header: " << uasset.header.EPackageFileTag << std::endl;
    std::cout << "Number of names: " << uasset.names.size() << std::endl;
    std::cout << "Number of imports: " << uasset.imports.size() << std::endl;
    std::cout << "Number of exports: " << uasset.exports.size() << std::endl;

    return 0;
}
