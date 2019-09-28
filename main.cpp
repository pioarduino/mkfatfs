//
//  main.cpp
//  mkfatfs
//
//  Created by Victor Mizikov on 20/09/2017.
//  Copyright (c) 2017 Victor Mizikov. All rights reserved.
//
#define TCLAP_SETBASE_ZERO 1
#define APP_VERSION "0.3.6"

#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <time.h>
#include <memory>
#include <cstdlib>
#include "tclap/CmdLine.h"
#include "tclap/UnlabeledValueArg.h"


//#if defined(__cplusplus)
//extern "C" {
//#endif
  #include <dirent.h>
//#if defined(__cplusplus)
//}
//#endif
#include <io.h>

#include "wear_levelling.h"
#include "esp_err.h"
#include "esp_vfs_fat.h"
//#include "esp_vfs.h" //do not include, dirent.h conflict

#include "fatfs/fatfs.h"
#include "fatfs/FatPartition.h"
#include "esp_log.h"

static const char *BASE_PATH = "/spiflash";
static const char* TAG = "main.c";

int g_debugLevel = 0;

enum Action { ACTION_NONE, ACTION_PACK, ACTION_UNPACK, ACTION_LIST, ACTION_VISUALIZE };
static Action s_action = ACTION_NONE;

static std::string s_dirName;
static std::string s_imageName;
static int s_imageSize;

static wl_handle_t s_wl_handle;
static FATFS* s_fs = NULL;

//----------------------------
int addDir(const char* name) {
    std::string fileName = name;
    fileName += "/.";

    if (g_debugLevel > 0) {
      std::cout << "creating dir: " << fileName << std::endl;
    }

    std::string nameInFat = BASE_PATH;
    nameInFat += name;
    int res = emulate_vfs_mkdir(nameInFat.c_str(), O_CREAT);
    if (res < 0) {
      std::cerr << "failed to create dir" << std::endl;
    }
    return 0;
}

/*
    path: source file
    name: des file
*/
//-----------------------------------------
int addFile(char* path_src, const char* path_des) {
    //spiffs_metadata_t meta;

    FILE* f_src = fopen(path_src, "rb"); //源文件
    if (!f_src) {
        std::cerr << "error: failed to open " << path_src << " for reading" << std::endl;
        return 1;
    }


    // std::string nameInFat = BASE_PATH;
    std::string nameInFat = path_des;

    const int flags = O_CREAT | O_TRUNC | O_RDWR;
    int fd = emulate_esp_vfs_open(nameInFat.c_str(), flags, 0);
    if (fd < 0) {
        std::cerr << "error: failed to open \"" << nameInFat << "\" for writing" << std::endl;
        return 0; //0 does not stop copying files
    }


    // read file size
    fseek(f_src, 0, SEEK_END);
    size_t size = ftell(f_src);
    fseek(f_src, 0, SEEK_SET);

    if (g_debugLevel > 0) {
        std::cout << "file size: " << size << std::endl;
    }

    size_t left = size;
    uint8_t data_byte;
    while (left > 0){
        if (1 != fread(&data_byte, 1, 1, f_src)) {
            std::cerr << "fread error!" << std::endl;
            fclose(f_src);
            emulate_esp_vfs_close(fd);
            return 1;
        }
        ssize_t res = emulate_esp_vfs_write(fd, &data_byte, 1);
        if (res < 0) {
            std::cerr << "esp_vfs_write() error" << std::endl;
            if (g_debugLevel > 0) {
                std::cout << "data left: " << left << std::endl;
            }
            fclose(f_src);
            emulate_esp_vfs_close(fd);
            return 1;
        }
        left -= 1;
    }

    emulate_esp_vfs_close(fd);

    fclose(f_src);

    return 0;
}

/*
//-------------------------------------------------------
int addFiles(const char* dirname, const char* subPath) {
    DIR *dir;
    struct dirent *ent;
    bool error = false;
    std::string dirPath = dirname;
    dirPath += subPath;   //需打包的文件夹， eg: "abc/"

    // Open directory
    if ((dir = opendir (dirPath.c_str())) != NULL) {

        // Read files from directory.
        while ((ent = readdir (dir)) != NULL) { //处理目录中的每一项
            // Ignore dir itself.
            if (ent->d_name[0] == '.')				
                continue;            	

            std::string fullpath = dirPath;
            fullpath += ent->d_name;
            struct stat path_stat;
            stat (fullpath.c_str(), &path_stat); //通过文件名filename获取文件信息

            if (!S_ISREG(path_stat.st_mode)) { //非常规文件
                // Check if path is a directory.
                if (S_ISDIR(path_stat.st_mode)) { 
                    // Prepare new sub path.
                    std::string newSubPath = subPath;
                    newSubPath += ent->d_name;
                    
                    addDir(newSubPath.c_str());
                    
                    newSubPath += "/";

                    if (addFiles(dirname, newSubPath.c_str()) != 0)
                    {
                        std::cerr << "Error for adding content from " << ent->d_name << "!" << std::endl;
                    }

                    continue;
                }
                else {
                    std::cerr << "skipping " << ent->d_name << std::endl;
                    continue;
                }
            }

            // Filepath with dirname as root folder.
            std::string filepath = subPath;
            filepath += ent->d_name;
            std::cout << "adding to image: " << filepath << std::endl;

            // Add File to image.
            if (addFile((char*)filepath.c_str(), fullpath.c_str()) != 0) {
                std::cerr << "error adding file!" << std::endl;
                error = true;
                if (g_debugLevel > 0) {
                    std::cout << std::endl;
                }
                break;
            }
        } // end while
        closedir (dir);
    }
    else {
        std::cerr << "warning: can't read source directory: \"" << dirPath << "\"" << std::endl;
        return 1;
    }

    return (error) ? 1 : 0;
}
*/

/* parkToRamFS()
    遍历源文件夹，拷贝到RAM fat file system
    params:
    sourceFiles:源文件兲路径
*/
bool parkToRamFS(const char* dirSrc, const char* dirDes)
{  
    DIR *dir;
    bool error = false;

    intptr_t handle;
    _finddata_t findData;
    std::string dir_OS = dirSrc;
    std::string dir_RAM_fs = dirDes;
    std::string dir_s;
    std::string dir_d;

    // Open directory
    if ((dir = opendir (dir_OS.c_str())) != NULL) 
    {
        closedir (dir);
    }
    else
    {
        std::cerr << "warning: can't read source directory: \"" << dir_OS.c_str() << "\"" << std::endl;
        return error;
    }
    
    std::string dir_root = dir_OS + "\\*.*";
    handle = _findfirst(dir_root.c_str(), &findData);
    if (handle == -1)        // 检查是否成功
        return error;

    do
    {
        dir_s = dir_OS + "\\" + findData.name;
        dir_d = dir_RAM_fs + "/" + findData.name;
        if (findData.attrib & _A_SUBDIR) //目录
        {
            if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
                continue;
            // dir_s = dir_OS + "\\" + findData.name;
            // dir_d = dir_RAM_fs + "/" + findData.name;
            std::cout << "\n" << "dir_RAM_fs: "<< dir_RAM_fs.c_str() << "  dir_OS: "<< dir_OS.c_str() << std::endl;
            std::cout << "Sub dir: "<< findData.name << "  full dir_s: "<< dir_s.c_str() << "   full dir_d: "<< dir_d.c_str() << "\n" << std::endl;
            emulate_vfs_mkdir(dir_d.c_str(), 1);
            parkToRamFS(dir_s.c_str(), dir_d.c_str()); //通过递归调用，实现进入各级子目录拷贝
        }
        else 
        {
            // Add File to image.
            if (addFile((char*)dir_s.c_str(), dir_d.c_str()) != 0) {
                std::cerr << "error adding file!" << "\n" << std::endl;
                error = true;
                if (g_debugLevel > 0) {
                    std::cout << std::endl;
                }
                break;
            }
            std::cout << "Pack file, dir_s: " << dir_s.c_str() << "dir_d: "<< dir_d.c_str() << "  file name: "<< findData.name << std::endl;
        }

    } while (_findnext(handle, &findData) == 0);

    _findclose(handle);    // 关闭搜索句柄
    return error;
}

/**
 * @brief Check if directory exists.
 * @param path Directory path.
 * @return True if exists otherwise false.
 *
 * @author Pascal Gollor (http://www.pgollor.de/cms/)
 */
//--------------------------------
bool dirExists(const char* path) {
    DIR *d = opendir(path);

    if (d) {
        closedir(d);
        return true;
    }

    return false;
}


/**
 * @brief Create directory if it not exists.
 * @param path Directory path.
 * @return True or false.
 *
 * @author Pascal Gollor (http://www.pgollor.de/cms/)
 */
//--------------------------------
bool dirCreate(const char* path) {
    // Check if directory also exists.
    if (dirExists(path)) {
	    return false;
    }

    // platform stuff...
#if defined(_WIN32)
    if (mkdir(path) != 0) {
#else
    if (mkdir(path, S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH) != 0) {
#endif
	    std::cerr << "Can not create directory!!!" << std::endl;
		return false;
    }

    return true;
}

/* unparkFileFromRamFS()
    path_src: source file, in RAM file system
    path_des: des file, to OS
*/
//-----------------------------------------
int unparkFileFromRamFS(const char* path_src, const char* path_des) 
{
    std::vector<uint8_t> temp_buf;

    const int flags = O_RDONLY; 
    int f_src = emulate_esp_vfs_open(path_src, flags, 0 ); //源文件
    if (f_src < 0) {
        std::cerr << "error: failed to open" << path_src << "\" for writing" << std::endl;
        return 0; //0 does not stop copying files
    }

    FILE* f_des = fopen(path_des, "wb");
    if (!f_des) {
        std::cerr << "error: failed to open " << path_des << " for reading" << std::endl;
        return 1;
    }


    // read file size
    size_t size = emulate_esp_vfs_lseek(f_src, 0, SEEK_END);
    // size_t size = ftell(f_src);
    emulate_esp_vfs_lseek(f_src, 0, SEEK_SET);
    std::cout << "file size: " << size << std::endl;
    if (g_debugLevel > 0) {
        std::cout << "file size: " << size << std::endl;
    }

    temp_buf.resize(size);
    // if(emulate_esp_vfs_read(f_src, (void *)&g_flashmem[0], size) < size);
    // {
    //     std::cout << "file read error! " << std::endl;
    //     return -1;
    // }
    emulate_esp_vfs_read(f_src, (void *)&temp_buf[0], size);
    fwrite ( (const void *)&temp_buf[0], 1, size, f_des );


    // size_t left = size;
    // uint8_t data_byte;
    // while (left > 0){
    //     if (1 != fread(&data_byte, 1, 1, f_des)) {
    //         std::cerr << "fread error!" << std::endl;
    //         fclose(f_des);
    //         emulate_esp_vfs_close(fd);
    //         return 1;
    //     }
    //     ssize_t res = emulate_esp_vfs_write(fd, &data_byte, 1);
    //     if (res < 0) {
    //         std::cerr << "esp_vfs_write() error" << std::endl;
    //         if (g_debugLevel > 0) {
    //             std::cout << "data left: " << left << std::endl;
    //         }
    //         fclose(f_src);
    //         emulate_esp_vfs_close(fd);
    //         return 1;
    //     }
    //     left -= 1;
    // }

    emulate_esp_vfs_close(f_src);
    fclose(f_des);

    return 0;
}

/* unparkFilesFromRamFS()
    遍历RAM文件系统，拷贝到指定文件夹
    params:
    dirDes:目标路径
*/
bool unparkFilesFromRamFS(const char* dirSrc, const char* dirDes)
{
    DIR *dir;
    struct dirent *ent;
    bool error = false;
    std::string dir_OS = dirDes;
    std::string dir_RAM_fs = dirSrc;
    std::string dir_s;
    std::string dir_d;
    std::cout << "dir_RAM_fs: "<< dir_RAM_fs.c_str() << "  dir_OS: "<< dir_OS.c_str() << std::endl;

    // Open directory
    // std::cout << "dir_RAM_fs: "<< dir_RAM_fs.c_str()  << std::endl;
    if ((dir = emulate_vfs_opendir(dir_RAM_fs.c_str())) != NULL) {

        // Read files from directory.
        while ((ent = emulate_vfs_readdir (dir)) != NULL) { //处理目录中的每一项
            // Ignore dir itself.         	
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            {
                std::cout << "ent->d_name: "<< ent->d_name  << std::endl;
                continue;
            }
               
            dir_s = dir_RAM_fs + "/" + ent->d_name;
            dir_d = dir_OS + "\\" + ent->d_name;
            std::cout << "RAM dir: "<< dir_s.c_str() << std::endl;
            struct stat path_stat;
            emulate_esp_vfs_stat (dir_s.c_str(), &path_stat); //通过文件名filename获取文件信息

            if (S_ISDIR(path_stat.st_mode)) { 
                std::cout << "Sub dir: "<< ent->d_name << "  full dir_s: "<< dir_s.c_str() << "   full dir_d: "<< dir_d.c_str() << "\n" << std::endl;
                dirCreate(dir_d.c_str());
                if (unparkFilesFromRamFS(dir_s.c_str(), dir_d.c_str()) != 0)
                {
                    std::cerr << "Error for unpark content from " << ent->d_name << "!" << std::endl;
                }
            }
            else if (S_ISREG(path_stat.st_mode)) //常规文件
            { 
                // Add File to image.
                if (unparkFileFromRamFS(dir_s.c_str(), dir_d.c_str()) != 0) {
                    std::cerr << "error unpark file!" << std::endl;
                    error = true;
                    if (g_debugLevel > 0) {
                        std::cout << std::endl;
                    }
                    break;
                }
            }
        } // end while
        emulate_vfs_closedir (dir);
    }
    else {
        std::cerr << "warning: can't read source directory: \"" << dir_RAM_fs.c_str() << "\"" << std::endl;
        return 1;
    }

    return (error) ? 1 : 0;
}

//-----------------
bool fatfsMount() {
  bool result;
  esp_vfs_fat_mount_config_t mountConfig;
  mountConfig.max_files = 4; //最大打开文件数量
  mountConfig.format_if_mount_failed = true;
  result = (ESP_OK == emulate_esp_vfs_fat_spiflash_mount(BASE_PATH, &mountConfig, &s_wl_handle, &s_fs, s_imageSize));

  return result;
}


//-------------------
bool fatfsUnmount() {
  bool result;

  result = (ESP_OK == emulate_esp_vfs_fat_spiflash_unmount(BASE_PATH, s_wl_handle));

  if (result) {
    if (g_debugLevel > 0) {
      std::cout << "Unmounted successfully" << std::endl;
    }
  } else {
    std::cerr << "Unmount failed" << std::endl;
  }

  return result;
}

//----------------
int actionPack() {
    int ret = 0; //0 - ok

    g_flashmem.resize(s_imageSize, 0xff);

    FILE* fdres = fopen(s_imageName.c_str(), "wb");  //打开目标打包文件(esp32文件系统bin)
    if (!fdres) {
        std::cerr << "error: failed to open image file" << std::endl;
        return 1;
    }

    if (fatfsMount()) {
      if (g_debugLevel > 0) {
        std::cout << "Mounted successfully" << std::endl;
      }
    } else {
      std::cerr << "Mount failed" << std::endl;
      return 1;
    }  

    // ret = addFiles(s_dirName.c_str());
    ret = parkToRamFS(s_dirName.c_str(), BASE_PATH); 
    fatfsUnmount();

    fwrite(&g_flashmem[0], 4, g_flashmem.size()/4, fdres); //把g_flashmem中的数据(RAM fat file system)写入目标打包文件，4字节对齐？
    fclose(fdres);

    if (g_debugLevel > 0) {
      std::cout << "Image file is written to \"" << s_imageName << "\"" << std::endl;
    }

    return ret;
}

static size_t getFileSize(FILE* fp)
{
    fseek(fp, 0L, SEEK_END);
    size_t size = (size_t) ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return size;
}

/**
 * @brief Unpack action.
 * @return 0 success, 1 error
 *
 * @author Pascal Gollor (http://www.pgollor.de/cms/)
 */
int actionUnpack(void)
{
    int ret = 0;

    // 1. open fatfs image file(bin) 
    FILE* f_src = fopen(s_imageName.c_str(), "rb");
    if (!f_src) {
        std::cerr << "error: failed to open image file" << std::endl;
        return 1;
    }

    if (s_imageSize == 0) {
        s_imageSize = getFileSize(f_src); //获取映像文件(bin)大小
    }

    // int err = checkArgs();
    // if (err != 0) {
    //     return err;
    // }

    g_flashmem.resize(s_imageSize, 0xff); //RAM模拟fatfs设置大小

    // 2. read content into s_flashmem, --> to RAM file system
    fread(&g_flashmem[0], 4, g_flashmem.size() / 4, f_src); //映像文件载入到RAM文件系统中。
    // close file handle
    fclose(f_src);

    // 3. mount RAM file system， so we can use vfs read file in RAM file system.
    if (fatfsMount()) {
      if (g_debugLevel > 0) {
        std::cout << "Mounted successfully" << std::endl;
      }
    } else {
      std::cerr << "Mount failed" << std::endl;
      return 1;
    } 
    // std::cout << "g_flashmem: "<< g_flashmem[3] << std::endl; //test
    // 4. fine dir_OS dir, if no create it.
    dirCreate(s_dirName.c_str());

    // 5. unpack files,s_dirName:RAM中提取文件后要存入的目标文件夹。
    if (!unparkFilesFromRamFS(BASE_PATH, s_dirName.c_str())) {
        ret = 1;
    }

    // unmount file system
    fatfsUnmount();

    return ret;
}

//---------------------------------------------
void processArgs(int argc, const char** argv) {
    TCLAP::CmdLine cmd("", ' ', APP_VERSION);
    TCLAP::ValueArg<std::string> packArg( "c", "create", "create fatFS image from a directory", true, "", "pack_dir");
    TCLAP::ValueArg<std::string> unpackArg( "u", "unpack", "unpack fatFS image to a directory", true, "", "dest_dir");
    TCLAP::SwitchArg listArg( "l", "list", "list files in fatFS image", false);
    TCLAP::SwitchArg visualizeArg( "i", "visualize", "visualize fatFS image", false);
    TCLAP::UnlabeledValueArg<std::string> outNameArg( "image_file", "fatFS image file", true, "", "image_file"  );
    TCLAP::ValueArg<int> imageSizeArg( "s", "size", "fs image size, in bytes", false, 0x10000, "number" );
    TCLAP::ValueArg<int> debugArg( "d", "debug", "Debug level. 0 means no debug output.", false, 0, "0-5" );

    cmd.add( imageSizeArg );
    cmd.add(debugArg);
    std::vector<TCLAP::Arg*> args = {&packArg, &unpackArg, &listArg, &visualizeArg};
    cmd.xorAdd( args );
    cmd.add( outNameArg );
    cmd.parse( argc, argv );

    if (debugArg.getValue() > 0) {
        std::cout << "Debug output enabled" << std::endl;
        g_debugLevel = debugArg.getValue();
    }

    if (packArg.isSet()) {
        s_dirName = packArg.getValue();
        s_action = ACTION_PACK;
    } else if (unpackArg.isSet()) {
        s_dirName = unpackArg.getValue();
        s_action = ACTION_UNPACK;
    } else if (listArg.isSet()) {
        s_action = ACTION_LIST;
    } else if (visualizeArg.isSet()) {
        s_action = ACTION_VISUALIZE;
    }

    s_imageName = outNameArg.getValue();
    s_imageSize = imageSizeArg.getValue();


}

//=======================================
int main(int argc, const char * argv[]) {

    try {
        processArgs(argc, argv);
    } catch(...) {
        std::cerr << "Invalid arguments" << std::endl;
        return 1;
    }

    switch (s_action) {
    case ACTION_PACK:
        return actionPack();
        break;
    case ACTION_UNPACK:
        return actionUnpack();
         break;
    case ACTION_LIST:

        break;
    case ACTION_VISUALIZE:

        break;
    default:
        break;
    }

    return 1;
}