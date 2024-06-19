#include "ModelLoader.hpp"

std::optional<ComplexModel> ModelLoader::load(const std::string& filename, unsigned int flags)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filename, flags);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    DEBUG("Failed to load file " << filename << ". Assimp error " << importer.GetErrorString() << '\n');
    return std::nullopt;
  }
  else
  {
    ComplexModel model;
    // contains single mesh by default. delete it since process function creates meshes
    model.meshes().pop_back();
    // calc extent to scale all vertices in range [-1, 1]
    calc_max_extent(scene->mRootNode, scene);
    process(scene->mRootNode, scene, model);
    return model;
  }
}

void ModelLoader::process(const aiNode* root, const aiScene* scene, ComplexModel& model)
{
  for (unsigned int i = 0; i < root->mNumMeshes; i++)
  {
    const aiMesh* inmesh = scene->mMeshes[root->mMeshes[i]];
    model.meshes().insert(model.meshes().end(), Mesh());
    Mesh& outmesh = model.meshes().back();
    
    const bool has_normals = inmesh->HasNormals();
    const bool has_texture_coords = inmesh->HasTextureCoords(0);

    // process vertices
    for (unsigned int vidx = 0; vidx < inmesh->mNumVertices; vidx++)
    {
      aiVector3D vert = inmesh->mVertices[vidx];
      vert /= m_max_extent;

      Vertex v;
      v.position = glm::vec3(vert.x, vert.y, vert.z);
      v.color = glm::vec4(1.f, 0.f, 0.f, 1.f);
      if (has_normals)
      {
        v.normal = glm::vec3(inmesh->mNormals[vidx].x, inmesh->mNormals[vidx].y, inmesh->mNormals[vidx].z);
      }
      if (has_texture_coords)
      {
        v.texture = glm::vec2(inmesh->mTextureCoords[0]->x, inmesh->mTextureCoords[0]->y);
      }
      outmesh.append_vertex(v);
    }

    // process faces
    assert(inmesh->mNumFaces > 0);
    for (unsigned int fidx = 0; fidx < inmesh->mNumFaces; fidx++)
    {
      aiFace face = inmesh->mFaces[fidx];
      // all must be triangulated for now
      assert(face.mNumIndices == 3);
      Face myface;
      myface.resize(face.mNumIndices);
      for (unsigned int i = 0; i < face.mNumIndices; i++)
      {
        myface.data[i] = face.mIndices[i];
      }
      outmesh.append_face(myface);
    }

    // TODO: textures + materials

  }

  for (unsigned int i = 0; i < root->mNumChildren; i++)
  {
    process(root->mChildren[i], scene, model);
  }
}

void ModelLoader::calc_max_extent(const aiNode* root, const aiScene* scene)
{
  for (unsigned int i = 0; i < root->mNumMeshes; i++)
  {
    const aiMesh* inmesh = scene->mMeshes[root->mMeshes[i]];
    const auto& bbox = inmesh->mAABB;
    const float max_extent = get_max_extent(bbox.mMin, bbox.mMax);
    m_max_extent = std::max(m_max_extent, max_extent);
  }
  for (unsigned int i = 0; i < root->mNumChildren; i++)
  {
    calc_max_extent(root->mChildren[i], scene);
  }
}

float ModelLoader::get_max_extent(const aiVector3D& min, const aiVector3D& max)
{
  const float x = std::max(std::abs(max.x), std::abs(min.x));
  const float y = std::max(std::abs(max.y), std::abs(min.y));
  const float z = std::max(std::abs(max.z), std::abs(min.z));
  return std::max(x, std::max(y, z));
} 
