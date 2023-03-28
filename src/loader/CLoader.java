package loader;

import arc.util.Log;
import mindustry.Vars;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;

public class CLoader {
    static String root="CMod";
    static String lib="eee";
    native void Load(String file);
    static {
        InputStream inputStream = ClassLoader.getSystemResourceAsStream(lib+".so");
        if(inputStream==null){
            Log.err("err no find so");
            throw  new RuntimeException("can't load lib");
        }
        File tempFile;
        try {
            tempFile = File.createTempFile(lib, ".so");
        } catch (IOException e) {
            Log.err("err createTempFile");
            throw new RuntimeException(e);
        }
        tempFile.deleteOnExit();
        try {
            Files.copy(inputStream, tempFile.toPath(), StandardCopyOption.REPLACE_EXISTING);
        } catch (IOException e) {
            Log.err("err copy file");
            throw new RuntimeException(e);
        }
        System.load(tempFile.getAbsolutePath());
    }
    void LoadMods(){
        var file= Vars.modDirectory.child(root);
        if(!file.exists()){
            file.mkdirs();
            Log.info("Init C modDirectory at {0}",file.absolutePath());
        }
        for(var i:file.list()){
            Load(i.absolutePath());
        }
    }

}
