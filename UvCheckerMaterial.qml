import QtQuick
import QtQuick3D

CustomMaterial {
    property TextureInput uvTexture: TextureInput {
        texture: Texture {
            source: "UVCheckerMap01.png"
        }
    }
    shadingMode: CustomMaterial.Unshaded
    fragmentShader: "UVChecker.frag"
    vertexShader: "UVChecker.vert"
}
