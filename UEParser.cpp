#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <exception>


struct ThumbnailIndex {
    std::string AssetClassName;
    std::string ObjectPathWithoutPackageName;
    int32_t FileOffset;
};

struct Thumbnail {
    int32_t ImageWidth;
    int32_t ImageHeight;
    std::string ImageFormat;
    int32_t ImageSizeData;
    std::vector<uint8_t> ImageData;
};

struct Tag {
    std::string Key;
    std::string Value;
};

struct AssetRegistryEntry {
    std::string ObjectPath;
    std::string ObjectClassName;
    std::vector<Tag> Tags;
};

struct AssetRegistryData {
    int64_t DependencyDataOffset;
    int32_t size;
    std::vector<AssetRegistryEntry> data;
};



// Custom exception class for handling parsing errors
class ParseException : public std::exception {
public:
    explicit ParseException(const std::string& message) : msg_(message) {}
    virtual const char* what() const noexcept { return msg_.c_str(); }
private:
    std::string msg_;
};

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
        int32_t EngineChangelist;
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

    struct GatherableTextData {
        std::string NamespaceName;
        struct SourceDataStruct {
            std::string SourceString;
            struct SourceStringMetaDataStruct {
                int32_t ValueCount;
                std::vector<std::string> Values;
            } SourceStringMetaData;
        } SourceData;
        struct SourceSiteContextStruct {
            std::string KeyName;
            std::string SiteDescription;
            uint32_t IsEditorOnly;
            uint32_t IsOptional;
            struct InfoMetaDataStruct {
                int32_t ValueCount;
                std::vector<std::string> Values;
            } InfoMetaData;
            struct KeyMetaDataStruct {
                int32_t ValueCount;
                std::vector<std::string> Values;
            } KeyMetaData;
        };
        std::vector<SourceSiteContextStruct> SourceSiteContexts;
    };

    Header header;
    std::vector<Name> names;
    std::vector<Import> imports;
    std::vector<Export> exports;
    std::vector<GatherableTextData> gatherableTextData;

    std::vector<ThumbnailIndex> thumbnailsIndex;
    std::vector<Thumbnail> thumbnails;

    Uasset() = default;
    AssetRegistryData assetRegistryData;
    bool parse(const std::vector<uint8_t>& bytes);
private:
    size_t currentIdx = 0;
    const std::vector<uint8_t>* bytesPtr = nullptr;

    uint16_t readUint16();
    int32_t readInt32();
    uint32_t readUint32();
    int64_t readInt64();
    int64_t readInt64Export();
    uint64_t readUint64();
    std::string readFStringAsUint64();
    std::string readFString();
    std::string readGuid();
    std::string readGuidString();
    std::vector<uint8_t> readCountBytes(int32_t count);
    
    void readAssetRegistryData();
    bool readHeader();
    void readNames();
    bool readGatherableTextData();
    void readImports();
    void readExports();
    void readThumbnails();
    std::string resolveFName(int32_t idx);
};

std::string Uasset::resolveFName(int32_t idx) {
    if (idx >= 0 && idx < names.size()) {
        return names[idx].Name;
    }
    return "";
}

bool Uasset::parse(const std::vector<uint8_t>& bytes) {
    currentIdx = 0;
    bytesPtr = &bytes;

    try {
        if (!readHeader()) {
            throw ParseException("Failed to read header");
        }

        readNames();

        if (!readGatherableTextData()) {
            throw ParseException("Failed to read gatherable text data");
        }

        readImports();
        readExports();
        readThumbnails();
        readAssetRegistryData();
        return true;
    } catch (const ParseException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

bool Uasset::readHeader() {
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
        std::cout << "SoftObjectPathsCount: " << header.SoftObjectPathsCount << std::endl;
        header.SoftObjectPathsOffset = readUint32();
        std::cout << "SoftObjectPathsOffset: " << header.SoftObjectPathsOffset << std::endl;
    }

    header.LocalizationId = readFString();
    std::cout << "LocalizationId: " << header.LocalizationId << std::endl;

    header.GatherableTextDataCount = readInt32();
    std::cout << "GatherableTextDataCount: " << header.GatherableTextDataCount << std::endl;
    header.GatherableTextDataOffset = readInt32();
    std::cout << "GatherableTextDataOffset: " << header.GatherableTextDataOffset << std::endl;

    header.ExportCount = readInt32();
    std::cout << "ExportCount: " << header.ExportCount << std::endl;
    header.ExportOffset = readInt32();
    std::cout << "ExportOffset: " << header.ExportOffset << std::endl;
    header.ImportCount = readInt32();
    std::cout << "ImportCount: " << header.ImportCount << std::endl;
    header.ImportOffset = readInt32();
    std::cout << "ImportOffset: " << header.ImportOffset << std::endl;
    header.DependsOffset = readInt32();
    std::cout << "DependsOffset: " << header.DependsOffset << std::endl;

    if (header.FileVersionUE4 >= 0x0154) { // VER_UE4_ADD_STRING_ASSET_REFERENCES_MAP
        header.SoftPackageReferencesCount = readInt32();
        std::cout << "SoftPackageReferencesCount: " << header.SoftPackageReferencesCount << std::endl;
        header.SoftPackageReferencesOffset = readInt32();
        std::cout << "SoftPackageReferencesOffset: " << header.SoftPackageReferencesOffset << std::endl;
    }

    if (header.FileVersionUE4 >= 0x0163) { // VER_UE4_ADDED_SEARCHABLE_NAMES
        header.SearchableNamesOffset = readInt32();
        std::cout << "SearchableNamesOffset: " << header.SearchableNamesOffset << std::endl;
    }

    header.ThumbnailTableOffset = readInt32();
    std::cout << "ThumbnailTableOffset: " << header.ThumbnailTableOffset << std::endl;
    header.Guid = readGuid();
    std::cout << "Guid: " << header.Guid << std::endl;

    if (header.FileVersionUE4 >= 0x0166) { // VER_UE4_ADDED_PACKAGE_OWNER
        header.PersistentGuid = readGuid();
        std::cout << "PersistentGuid: " << header.PersistentGuid << std::endl;
    }

    if (header.FileVersionUE4 >= 0x0166 && header.FileVersionUE4 < 0x0183) { // VER_UE4_NON_OUTER_PACKAGE_IMPORT
        header.OwnerPersistentGuid = readGuid();
        std::cout << "OwnerPersistentGuid: " << header.OwnerPersistentGuid << std::endl;
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
    } else {
        header.EngineChangelist = readInt32();
    }

    if (header.FileVersionUE4 >= 0x0175) { // VER_UE4_PACKAGE_SUMMARY_HAS_COMPATIBLE_ENGINE_VERSION
        header.CompatibleWithEngineVersion = std::to_string(readUint16()) + "." +
            std::to_string(readUint16()) + "." +
            std::to_string(readUint16()) + "-" +
            std::to_string(readUint32()) + "+" +
            readFString();
    } else {
        header.CompatibleWithEngineVersion = header.SavedByEngineVersion;
    }

    header.CompressionFlags = readUint32();

    int32_t compressedChunksCount = readInt32();
    if (compressedChunksCount > 0) {
        throw ParseException("Asset compressed");
    }

    header.PackageSource = readUint32();
    header.AdditionalPackagesToCookCount = readUint32();
    if (header.AdditionalPackagesToCookCount > 0) {
        throw ParseException("AdditionalPackagesToCook has items");
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
    } else if (header.FileVersionUE4 >= 0x0192) { // VER_UE4_ADDED_CHUNKID_TO_ASSETDATA_AND_UPACKAGE
        header.ChunkID = readInt32();
    }

    if (header.FileVersionUE4 >= 0x0194) { // VER_UE4_PRELOAD_DEPENDENCIES_IN_COOKED_EXPORTS
        header.PreloadDependencyCount = readInt32();
        header.PreloadDependencyOffset = readInt32();
    } else {
        header.PreloadDependencyCount = -1;
        header.PreloadDependencyOffset = 0;
    }

    if (header.FileVersionUE5 >= 0x0196) { // VER_UE5_NAMES_REFERENCED_FROM_EXPORT_DATA
        header.NamesReferencedFromExportDataCount = readInt32();
    }

    if (header.FileVersionUE5 >= 0x0197) { // VER_UE5_PAYLOAD_TOC
        header.PayloadTocOffset = readInt64();
    } else {
        header.PayloadTocOffset = -1;
    }

    if (header.FileVersionUE5 >= 0x0198) { // VER_UE5_DATA_RESOURCES
        header.DataResourceOffset = readInt32();
    }

    return true;
}

void Uasset::readNames() {
    currentIdx = header.NameOffset;
    for (int32_t i = 0; i < header.NameCount; ++i) {
        Name name;
        name.Name = readFString();
        name.NonCasePreservingHash = readUint16();
        name.CasePreservingHash = readUint16();
        names.push_back(name);
    }
}

bool Uasset::readGatherableTextData() {
    currentIdx = header.GatherableTextDataOffset;
    for (int32_t i = 0; i < header.GatherableTextDataCount; ++i) {
        GatherableTextData gatherableTextData;

        gatherableTextData.NamespaceName = readFString();
        gatherableTextData.SourceData.SourceString = readFString();
        gatherableTextData.SourceData.SourceStringMetaData.ValueCount = readInt32();

        if (gatherableTextData.SourceData.SourceStringMetaData.ValueCount > 0) {
            throw ParseException("Unsupported SourceStringMetaData from readGatherableTextData");
        }

        int32_t countSourceSiteContexts = readInt32();
        for (int32_t j = 0; j < countSourceSiteContexts; ++j) {
            GatherableTextData::SourceSiteContextStruct sourceSiteContext;
            sourceSiteContext.KeyName = readFString();
            sourceSiteContext.SiteDescription = readFString();
            sourceSiteContext.IsEditorOnly = readUint32();
            sourceSiteContext.IsOptional = readUint32();

            sourceSiteContext.InfoMetaData.ValueCount = readInt32();
            if (sourceSiteContext.InfoMetaData.ValueCount > 0) {
                throw ParseException("Unsupported SourceSiteContexts.InfoMetaData from readGatherableTextData");
            }

            sourceSiteContext.KeyMetaData.ValueCount = readInt32();
            if (sourceSiteContext.KeyMetaData.ValueCount > 0) {
                throw ParseException("Unsupported SourceSiteContexts.KeyMetaData from readGatherableTextData");
            }

            gatherableTextData.SourceSiteContexts.push_back(sourceSiteContext);
        }

        this->gatherableTextData.push_back(gatherableTextData);
    }
    return true;
}


void Uasset::readImports() {
    currentIdx = header.ImportOffset;
    imports.clear();

    for (int32_t i = 0; i < header.ImportCount; ++i) {
        Import importA;

        // Read indices and resolve names
        int64_t classPackageIdx = readInt64();
        int64_t classNameIdx = readInt64();
        importA.outerIndex = readInt32();
        int64_t objectNameIdx = readInt64();

        importA.classPackage = resolveFName(classPackageIdx);
        importA.className = resolveFName(classNameIdx);
        importA.objectName = resolveFName(objectNameIdx);

        if (header.FileVersionUE4 >= 0x0166) { // VER_UE4_NON_OUTER_PACKAGE_IMPORT
            int64_t packageNameIdx = readInt64();
            importA.packageName = resolveFName(packageNameIdx);
        }
        else {
            importA.packageName = "";
        }

        if (header.FileVersionUE5 >= 0x0197) { // VER_UE5_OPTIONAL_RESOURCES
            importA.bImportOptional = readInt32();
        }
        else {
            importA.bImportOptional = 0;
        }

        imports.push_back(importA);
    }
}


std::string Uasset::readFStringAsUint64() {
    uint64_t value = readUint64();
    return std::to_string(value);
}

uint64_t Uasset::readUint64() {
    if (currentIdx + sizeof(uint64_t) > bytesPtr->size()) {
        throw ParseException("Out of bounds read (uint64)");
    }
    uint64_t val;
    std::memcpy(&val, &(*bytesPtr)[currentIdx], sizeof(val));
    currentIdx += sizeof(val);
    return val;
}


std::string Uasset::readGuidString() {
    if (currentIdx + 16 > bytesPtr->size()) {
        throw ParseException("Out of bounds read (GuidString)");
    }

    std::ostringstream ss;
    for (int i = 0; i < 16; ++i) {
        if (i == 4 || i == 6 || i == 8 || i == 10)
            ss << '-';
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>((*bytesPtr)[currentIdx + i]);
    }
    currentIdx += 16;
    std::string guid = ss.str();
    std::transform(guid.begin(), guid.end(), guid.begin(), ::toupper);
    return guid;
}

void Uasset::readExports() {
    currentIdx = header.ExportOffset;
    exports.clear();

    for (int32_t i = 0; i < header.ExportCount; ++i) {
        Export exportData;

        exportData.classIndex = readInt32();
        exportData.superIndex = readInt32();

        if (header.FileVersionUE4 >= 0x00AD) { // VER_UE4_TEMPLATEINDEX_IN_COOKED_EXPORTS
            exportData.templateIndex = readInt32();
        }
        else {
            exportData.templateIndex = 0;
        }

        exportData.outerIndex = readInt32();
        int32_t objectNameIdx = readInt32();
        exportData.objectName = resolveFName(objectNameIdx);

        currentIdx += 4;
        exportData.objectFlags = readUint32();
        exportData.serialSize = readInt64Export();
        exportData.serialOffset = readInt64Export();
        exportData.bForcedExport = readInt32();
        exportData.bNotForClient = readInt32();
        exportData.bNotForServer = readInt32();
        exportData.packageGuid = readGuidString(); // Use the corrected method
        exportData.packageFlags = readUint32();

        if (header.FileVersionUE4 >= 0x00AC) { // VER_UE4_LOAD_FOR_EDITOR_GAME
            exportData.bNotAlwaysLoadedForEditorGame = readInt32();
        }
        else {
            exportData.bNotAlwaysLoadedForEditorGame = 0;
        }

        if (header.FileVersionUE4 >= 0x00AE) { // VER_UE4_COOKED_ASSETS_IN_EDITOR_SUPPORT
            exportData.bIsAsset = readInt32();
        }
        else {
            exportData.bIsAsset = 0;
        }

        if (header.FileVersionUE5 >= 0x0197) { // VER_UE5_OPTIONAL_RESOURCES
            exportData.bGeneratePublicHash = readInt32();
        }
        else {
            exportData.bGeneratePublicHash = 0;
        }

        if (header.FileVersionUE4 >= 0x0194) { // VER_UE4_PRELOAD_DEPENDENCIES_IN_COOKED_EXPORTS
            exportData.firstExportDependency = readInt32();
            exportData.serializationBeforeSerializationDependencies = readInt32();
            exportData.createBeforeSerializationDependencies = readInt32();
            exportData.serializationBeforeCreateDependencies = readInt32();
            exportData.createBeforeCreateDependencies = readInt32();
        }
        else {
            exportData.firstExportDependency = 0;
            exportData.serializationBeforeSerializationDependencies = 0;
            exportData.createBeforeSerializationDependencies = 0;
            exportData.serializationBeforeCreateDependencies = 0;
            exportData.createBeforeCreateDependencies = 0;
        }

        exports.push_back(exportData);
    }
}


void Uasset::readThumbnails() {
    currentIdx = header.ThumbnailTableOffset;

    int32_t count = readInt32();
    thumbnailsIndex.clear();
    thumbnails.clear();

    for (int32_t idx = 0; idx < count; ++idx) {
        ThumbnailIndex index;
        index.AssetClassName = readFString();
        index.ObjectPathWithoutPackageName = readFString();
        index.FileOffset = readInt32();
        thumbnailsIndex.push_back(index);
    }

    for (int32_t idx = 0; idx < count; ++idx) {
        currentIdx = thumbnailsIndex[idx].FileOffset;

        Thumbnail thumbnail;
        thumbnail.ImageWidth = readInt32();
        thumbnail.ImageHeight = readInt32();
        thumbnail.ImageFormat = "PNG";

        if (thumbnail.ImageHeight < 0) {
            thumbnail.ImageFormat = "JPEG";
            thumbnail.ImageHeight = -thumbnail.ImageHeight;
        }

        thumbnail.ImageSizeData = readInt32();
        if (thumbnail.ImageSizeData > 0) {
            thumbnail.ImageData = readCountBytes(thumbnail.ImageSizeData);
        }

        thumbnails.push_back(thumbnail);
    }
}

void Uasset::readAssetRegistryData() {
    currentIdx = header.AssetRegistryDataOffset;

    int32_t nextOffset = header.TotalHeaderSize;
    if (header.WorldTileInfoDataOffset > 0) {
        nextOffset = header.WorldTileInfoDataOffset;
    }

    assetRegistryData.size = nextOffset - header.AssetRegistryDataOffset;

    assetRegistryData.DependencyDataOffset = readInt64();

    int32_t count = readInt32();
    assetRegistryData.data.clear();
    for (int32_t idx = 0; idx < count; ++idx) {
        AssetRegistryEntry entry;
        entry.ObjectPath = readFString();
        entry.ObjectClassName = readFString();

        int32_t countTag = readInt32();
        for (int32_t idxTag = 0; idxTag < countTag; ++idxTag) {
            Tag tag;
            tag.Key = readFString();
            tag.Value = readFString();
            entry.Tags.push_back(tag);
        }

        assetRegistryData.data.push_back(entry);
    }
}



std::vector<uint8_t> Uasset::readCountBytes(int32_t count) {
    if (currentIdx + count > bytesPtr->size()) {
        throw std::runtime_error("Out of bounds read (count bytes)");
    }
    std::vector<uint8_t> bytes((*bytesPtr).begin() + currentIdx, (*bytesPtr).begin() + currentIdx + count);
    currentIdx += count;
    return bytes;
}

uint16_t Uasset::readUint16() {
    if (currentIdx + sizeof(uint16_t) > bytesPtr->size()) {
        throw ParseException("Out of bounds read (uint16)");
    }
    uint16_t val;
    std::memcpy(&val, &(*bytesPtr)[currentIdx], sizeof(val));
    currentIdx += sizeof(val);
    return val;
}

int32_t Uasset::readInt32() {
    if (currentIdx + sizeof(int32_t) > bytesPtr->size()) {
        throw ParseException("Out of bounds read (int32)");
    }
    int32_t val;
    std::memcpy(&val, &(*bytesPtr)[currentIdx], sizeof(val));
    currentIdx += sizeof(val);
    return val;
}

uint32_t Uasset::readUint32() {
    if (currentIdx + sizeof(uint32_t) > bytesPtr->size()) {
        throw ParseException("Out of bounds read (uint32)");
    }
    uint32_t val;
    std::memcpy(&val, &(*bytesPtr)[currentIdx], sizeof(val));
    currentIdx += sizeof(val);
    return val;
}

int64_t Uasset::readInt64() {
    if (currentIdx + sizeof(int64_t) > bytesPtr->size()) {
        throw ParseException("Out of bounds read (int64)");
    }
    int64_t val;
    std::memcpy(&val, &(*bytesPtr)[currentIdx], sizeof(val));
    currentIdx += sizeof(val);
    return val;
}

int64_t Uasset::readInt64Export() {
    if (currentIdx + sizeof(int64_t) > bytesPtr->size()) {
        throw std::runtime_error("Out of bounds read (int64)");
    }
    uint8_t b0 = (*bytesPtr)[currentIdx];
    uint8_t b1 = (*bytesPtr)[currentIdx + 1];
    uint8_t b2 = (*bytesPtr)[currentIdx + 2];
    uint8_t b3 = (*bytesPtr)[currentIdx + 3];
    uint8_t b4 = (*bytesPtr)[currentIdx + 4];
    uint8_t b5 = (*bytesPtr)[currentIdx + 5];
    uint8_t b6 = (*bytesPtr)[currentIdx + 6];
    uint8_t b7 = (*bytesPtr)[currentIdx + 7];
    currentIdx += sizeof(int64_t);
    return (int64_t(b0) | (int64_t(b1) << 8) | (int64_t(b2) << 16) | (int64_t(b3) << 24) |
        (int64_t(b4) << 32) | (int64_t(b5) << 40) | (int64_t(b6) << 48) | (int64_t(b7) << 56));
}




std::string Uasset::readFString() {
    int32_t length = readInt32();
    if (length == 0) return "";
    if (length > 0) {
        if (currentIdx + length > bytesPtr->size()) {
            throw ParseException("Out of bounds read (FString)");
        }
        std::string str((*bytesPtr).begin() + currentIdx, (*bytesPtr).begin() + currentIdx + length - 1);
        currentIdx += length;
        return str;
    } else {
        length = -length * 2;
        if (currentIdx + length > bytesPtr->size()) {
            throw ParseException("Out of bounds read (FString)");
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
        throw ParseException("Out of bounds read (Guid)");
    }
    std::memcpy(guid, &(*bytesPtr)[currentIdx], sizeof(guid));
    currentIdx += sizeof(guid);
    return guidToString(guid);
}

void printImports(const Uasset& uasset) {
    std::cout << "Imports:" << std::endl;
    for (size_t i = 0; i < uasset.imports.size(); ++i) {
        const auto& importA = uasset.imports[i];
        std::cout << "Import #" << (i + 1) << ":" << std::endl;
        std::cout << "  classPackage: " << importA.classPackage << std::endl;
        std::cout << "  className: " << importA.className << std::endl;
        std::cout << "  outerIndex: " << importA.outerIndex << std::endl;
        std::cout << "  objectName: " << importA.objectName << std::endl;
        std::cout << "  packageName: " << importA.packageName << std::endl;
        std::cout << "  bImportOptional: " << importA.bImportOptional << std::endl;
    }
}

void printExports(const Uasset& uasset) {
    std::cout << "Exports:" << std::endl;
    for (size_t i = 0; i < uasset.exports.size(); ++i) {
        const auto& exportA = uasset.exports[i];
        std::cout << "Export #" << (i + 1) << ":" << std::endl;
        std::cout << "  classIndex: " << exportA.classIndex << std::endl;
        std::cout << "  superIndex: " << exportA.superIndex << std::endl;
        std::cout << "  templateIndex: " << exportA.templateIndex << std::endl;
        std::cout << "  outerIndex: " << exportA.outerIndex << std::endl;
        std::cout << "  objectName: " << exportA.objectName << std::endl;
        std::cout << "  objectFlags: " << exportA.objectFlags << std::endl;
        std::cout << "  serialSize: " << exportA.serialSize << std::endl;
        std::cout << "  serialOffset: " << exportA.serialOffset << std::endl;
        std::cout << "  bForcedExport: " << exportA.bForcedExport << std::endl;
        std::cout << "  bNotForClient: " << exportA.bNotForClient << std::endl;
        std::cout << "  bNotForServer: " << exportA.bNotForServer << std::endl;
        std::cout << "  packageGuid: " << exportA.packageGuid << std::endl;
        std::cout << "  packageFlags: " << exportA.packageFlags << std::endl;
        std::cout << "  bNotAlwaysLoadedForEditorGame: " << exportA.bNotAlwaysLoadedForEditorGame << std::endl;
        std::cout << "  bIsAsset: " << exportA.bIsAsset << std::endl;
        std::cout << "  bGeneratePublicHash: " << exportA.bGeneratePublicHash << std::endl;
        std::cout << "  firstExportDependency: " << exportA.firstExportDependency << std::endl;
        std::cout << "  serializationBeforeSerializationDependencies: " << exportA.serializationBeforeSerializationDependencies << std::endl;
        std::cout << "  createBeforeSerializationDependencies: " << exportA.createBeforeSerializationDependencies << std::endl;
        std::cout << "  serializationBeforeCreateDependencies: " << exportA.serializationBeforeCreateDependencies << std::endl;
        std::cout << "  createBeforeCreateDependencies: " << exportA.createBeforeCreateDependencies << std::endl;
        for (size_t j = 0; j < exportA.data.size(); ++j) {
            std::cout << "  data[" << j << "]: " << exportA.data[j] << std::endl;
        }
    }
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

    // Print names
    for (const auto& name : uasset.names) {
        std::cout << std::endl;
        std::cout << "Name: " << name.Name << std::endl;
        std::cout << "NonCasePreservingHash: " << name.NonCasePreservingHash << std::endl;
        std::cout << "CasePreservingHash: " << name.CasePreservingHash << std::endl;
    }

    // Print imports
    printImports(uasset);

    // Print exports
    printExports(uasset);

    // Access and print thumbnail data
    for (const auto& thumbnail : uasset.thumbnails) {
        std::cout << "Thumbnail:" << std::endl;
        std::cout << "  Width: " << thumbnail.ImageWidth << std::endl;
        std::cout << "  Height: " << thumbnail.ImageHeight << std::endl;
        std::cout << "  Format: " << thumbnail.ImageFormat << std::endl;
        std::cout << "  Data Size: " << thumbnail.ImageSizeData << std::endl;
    }
    
    // Access and print asset registry data
    std::cout << "Asset Registry Data Size: " << uasset.assetRegistryData.size << std::endl;
    std::cout << "Dependency Data Offset: " << uasset.assetRegistryData.DependencyDataOffset << std::endl;

    for (const auto& entry : uasset.assetRegistryData.data) {
        std::cout << "Object Path: " << entry.ObjectPath << std::endl;
        std::cout << "Object Class Name: " << entry.ObjectClassName << std::endl;
        for (const auto& tag : entry.Tags) {
            std::cout << "  Tag Key: " << tag.Key << ", Tag Value: " << tag.Value << std::endl;
        }
    }

    
    
    return 0;
}
