package loader;

import mindustry.mod.Mod;

public class main extends Mod {
    @Override
    public void init() {
        CLoader loader=new CLoader();
        loader.LoadMods();
    }
}
