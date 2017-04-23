import sys
import copy
import xml.etree.ElementTree as etree
from enum import Enum

tree = etree.parse(sys.argv[1])
root = tree.getroot()

converted_root = etree.Element("Scene")
converted_cameras = etree.SubElement(converted_root, "Cameras")

ray_eps = root.find("ShadowRayEpsilon")

if ray_eps is None:
    etree.SubElement(converted_root, "ShadowRayEpsilon").text = str(1e-3)
else:
    converted_root.append(copy.deepcopy(ray_eps))

test_eps = root.find("IntersectionTestEpsilon")
if test_eps is None:
    etree.SubElement(converted_root, "IntersectionTestEpsilon").text = str(1e-3)
else:
    converted_root.append(copy.deepcopy(test_eps))

for cam in root.iterfind("Camera"):
    if not (cam.find("FocusDistance")) is None:
        cam.attrib["type"] = "aperture"
    else:
        cam.attrib["type"] = "pinhole"
    converted_cameras.append(copy.deepcopy(cam))

converted_root.append(copy.deepcopy(root.find("MaxRecursionDepth")))
converted_root.append(copy.deepcopy(root.find("BackgroundColor")))
converted_root.append(copy.deepcopy(root.find("Lights"))) # lights need no modification

if not (root.find("Transformations") is None):
    converted_root.append(copy.deepcopy(root.find("Transformations"))) # lights need no modification
else:
    etree.SubElement(converted_root, "Transformations")

#converted_root.append(copy.deepcopy(root.find("Materials"))) # materials are almost the same

new_mats = etree.SubElement(converted_root, "Materials")

class decal_modes(Enum):
    BlendCoeff = 0
    ReplaceCoeff = 1
    ReplaceComplete = 2

def get_decal_mode(text):
    if text == "replace_kd":
        return decal_modes.ReplaceCoeff
    if text == "blend_kd":
        return decal_modes.BlendCoeff
    if text == "replace_all":
        return decal_modes.ReplaceComplete


class texture(object):
    decal_mode = decal_modes.ReplaceComplete
    id = 0

    def to_xml(self, elem):
        elem.attrib["id"] = str(self.id)

class perlin_types(Enum):
    Vein = 1
    Patch = 2

class perlin_data(texture):
    appearance = perlin_types.Vein
    scale_factor = 1

    def to_xml(self, elem):
        elem = etree.Element("Perlin")

        super(perlin_data, self).to_xml(elem)
        etree.SubElement(elem, "Appearance").text = str(self.appearance)
        etree.SubElement(elem, "ScalingFactor").text = str(self.scale_factor)

        return elem


class sampling_mode(Enum):
    Nearest = 0
    Bilinear = 1

class image_data(texture):
    path = ""
    scaling = 1
    sampling = sampling_mode.Nearest

    def to_xml(self, elem):
        elem = etree.Element("Image")

        super(image_data, self).to_xml(elem)
        etree.SubElement(elem, "Path").text = self.path
        etree.SubElement(elem, "Scaling").text = str(self.scaling)
        etree.SubElement(elem, "Sampling").text = str(self.sampling)

        return elem

def parse_texture(elem):
    res = texture()

    if elem.find("ImageName").text == "perlin":
        res = perlin_data()
        res.appearance = perlin_types.Vein if elem.find("Appearance").text == "vein" else perlin_types.Patch
        res.scale_factor = float(elem.find("ScalingFactor").text)
    else:
        res = image_data()
        res.sampling = sampling_mode.Nearest if elem.find("Interpolation").text == "nearest" else sampling_mode.Bilinear
        res.path = elem.find("ImageName").text
        res.scaling = 255

    res.decal_mode = get_decal_mode(elem.find("DecalMode").text)
    res.id = int(elem.attrib["id"])
    return res

textures = {}

if not root.find("Textures") is None:
    for tex in root.find("Textures"):
        t = parse_texture(tex)
        textures[t.id] = t

new_textures = etree.SubElement(converted_root, "Textures")

for tex in textures:
    new_textures.append(textures[tex].to_xml(None))

materials = {}
custom_mat_id = 1000

for mat in root.find("Materials"):
    if "shader" in mat.attrib:
        new_mats.append(copy.deepcopy(mat))
        continue

    if not mat.find("refractionindex") is None:
        if not mat.find("refractionindex").text == "1" and not mat.find("transparency") == "0 0 0":
            mat.attrib["shader"] = "glass"
            new_mats.append(copy.deepcopy(mat))
            continue

    if not mat.find("Roughness") is None:
        if not mat.find("Roughness").text == "0":
            mat.attrib["shader"] = "metal"
            new_mats.append(copy.deepcopy(mat))
            continue

    if not mat.find("MirrorReflectance") is None:
        if not mat.find("MirrorReflectance").text == "0 0 0":
            mat.attrib["shader"] = "mirror"
            new_mats.append(copy.deepcopy(mat))
            continue

    mat.attrib["shader"] = "ceng795"

    materials[int(mat.attrib["id"])] = copy.deepcopy(mat)

def group(lst, n):
    return zip(*[lst[i::n] for i in range(n)])

vertexdata_text = root.find("VertexData").text
vertex_pos = [float(x) for x in vertexdata_text.split()]
vertices = group(vertex_pos, 3)

uvs = None
if not root.find("TexCoordData") is None:
    uv_data_text = root.find("TexCoordData").text
    uv_pos = [float(x) for x in uv_data_text.split()]
    uvs = group(uv_pos, 2)

buffers = etree.SubElement(converted_root, "Buffers")

next_v_buf_id = 0
next_i_buf_id = 0

scn_max = map(max, zip(*vertices))
scn_min = map(min, zip(*vertices))

scn_center =[sum(x) / 2 for x in zip(scn_min, scn_max)]
scn_extent =[max(0.01, y - x) for (x, y) in zip(scn_min, scn_max)]

max_radius = 0

def add_mesh_data(v_buffer, i_buffer, uv = None):
    vbuf = etree.SubElement(buffers, "VertexBuffer")
    v_buffer_txt = "\n".join(map(lambda x : " ".join(map(str, x)), v_buffer))

    ibuf = etree.SubElement(buffers, "IndexBuffer")
    i_buffer_txt = "\n".join(map(lambda x : " ".join(map(str, x)), i_buffer))

    uv_buf, uv_txt = None, None
    if not uv is None:
        uv_buf = etree.SubElement(buffers, "UVertexBuffer")
        uv_txt = "\n".join(map(lambda x : " ".join(map(str, x)), uv))

    global next_v_buf_id
    global next_i_buf_id

    v_buf_id = next_v_buf_id
    next_v_buf_id += 1
    i_buf_id = next_i_buf_id
    next_i_buf_id += 1

    uv_buf_id = None
    if not uv is None:
        uv_buf_id = next_v_buf_id
        next_v_buf_id += 1

    vbuf.attrib["id"] = str(v_buf_id)
    vbuf.text = v_buffer_txt

    ibuf.attrib["id"] = str(i_buf_id)
    ibuf.text = i_buffer_txt

    if not uv is None:
        uv_buf.attrib["id"] = str(uv_buf_id)
        uv_buf.text = uv_txt

    return (v_buf_id, i_buf_id, uv_buf_id)

# parse objects

objects = root.find("Objects")
new_objects = etree.SubElement(converted_root, "Objects")

for sphere in objects.iterfind("Sphere"):
    mat_id = int(sphere.find("Material").text)

    if not sphere.find("Texture") is None:
        texture_id = int(sphere.find("Texture").text)
        new_mat_id = custom_mat_id + 1
        custom_mat_id = custom_mat_id + 1
        materials[new_mat_id] = copy.deepcopy(materials[mat_id])
        materials[new_mat_id].attrib["id"] = str(new_mat_id)
        mat_id = new_mat_id
        materials[mat_id].find("DiffuseReflectance").attrib["tex_id"] = str(texture_id)
        materials[mat_id].find("DiffuseReflectance").attrib["tex_mode"] = str(textures[texture_id].decal_mode)

    sphere.find("Material").text = str(mat_id)

    new_elem = copy.deepcopy(sphere)
    max_radius = max(max_radius, float(new_elem.find("Radius").text))
    new_elem.find("Center").text = " ".join(map(str, vertices[int(sphere.find("Center").text) - 1]))
    if (new_elem.find("Transformations") is None):
        etree.SubElement(new_elem, "Transformations")
    new_objects.append(new_elem)

scn_extent = [sum(x) for x in zip(scn_extent, (max_radius, max_radius, max_radius))]

converted_root.attrib["center"] = " ".join(map(str, scn_center))
converted_root.attrib["extent"] = " ".join(map(str, scn_extent))

def add_mesh(mat_id, face_elem, tex_id = None):
    new_elem = etree.Element("Mesh")
    etree.SubElement(new_elem, "Material").text = str(mat_id)

    v_offset = int(face_elem.attrib["vertexOffset"])
    face_indices = [int(x) + v_offset for x in face_elem.text.split()]
    faces = group(face_indices, 3)

    used_vertices = []
    mapping = {}

    indices = set(face_indices)

    for index in indices:
        mapping[index] = len(used_vertices)
        used_vertices.append(vertices[index - 1])

    faces = map(lambda (a, b, c): (mapping[a], mapping[b], mapping[c]), faces)

    used_uvs = None
    if not tex_id is None:
        uv_offset = 0
        if ("textureOffset" in face_elem.attrib):
            uv_offset = int(face_elem.attrib["textureOffset"])
        uv_indices = [int(x) + uv_offset for x in face_elem.text.split()]
        uv_faces = group(uv_indices, 2)

        uv_mapping = {}
        used_uvs = []

        uv_indices = set(uv_indices)
        for index in uv_indices:
            uv_mapping[index] = len(used_uvs)
            used_uvs.append(uvs[index - 1])

        face_uvs = map(lambda (a, b): (uv_mapping[a], uv_mapping[b]), uv_faces)

    (v_id, i_id, uv_id) = add_mesh_data(used_vertices, faces, used_uvs)

    etree.SubElement(new_elem, "VertexBuffer").attrib["id"] = str(v_id)
    etree.SubElement(new_elem, "IndexBuffer").attrib["id"] = str(i_id)

    if not tex_id is None:
        etree.SubElement(new_elem, "UvBuffer").attrib["id"] = str(uv_id)

    return new_elem

meshes = {}

for mesh in objects.iterfind("Mesh"):
    faces = mesh.find("Faces")

    if not "vertexOffset" in faces.attrib:
        faces.attrib["vertexOffset"] = "0"

    mat_id = int(mesh.find("Material").text)

    texture_id = None
    if not mesh.find("Texture") is None:
        texture_id = int(mesh.find("Texture").text)
        new_mat_id = custom_mat_id + 1
        custom_mat_id = custom_mat_id + 1
        materials[new_mat_id] = copy.deepcopy(materials[mat_id])
        materials[new_mat_id].attrib["id"] = str(new_mat_id)
        mat_id = new_mat_id
        materials[mat_id].find("DiffuseReflectance").attrib["tex_id"] = str(texture_id)
        materials[mat_id].find("DiffuseReflectance").attrib["tex_mode"] = str(textures[texture_id].decal_mode)

    new_elem = add_mesh(mat_id, faces, texture_id)
    if not "shadingMode" in mesh.attrib:
        mesh.attrib["shadingMode"] = "flat"

    mesh.attrib["instanced"] = str(False)
    new_elem.attrib = mesh.attrib

    if mesh.find("Transformations") is None:
        etree.SubElement(new_elem, "Transformations").text = " "
    else:
        new_elem.append(copy.deepcopy(mesh.find("Transformations")))

    meshes[mesh.attrib["id"]] = new_elem

    new_objects.append(new_elem)

for mesh_inst in objects.iterfind("MeshInstance"):
    base_id = mesh_inst.attrib["baseMeshId"]
    base_mesh = copy.deepcopy(meshes[base_id])

    new_transforms = mesh_inst.find("Transformations").text if not mesh_inst.find("Transformations") is None else ""
    base_mesh.find("Transformations").text = base_mesh.find("Transformations").text + " " + new_transforms

    del base_mesh.attrib["id"]
    base_mesh.attrib["instanced"] = str(True)
    new_objects.append(base_mesh)

for tri in objects.iterfind("Triangle"):
    faces = tri.find("Indices")

    if not "vertexOffset" in faces.attrib:
        faces.attrib["vertexOffset"] = "0"

    mat_id = int(mesh.find("Material").text)
    texture_id = None
    if not mesh.find("Texture") is None:
        texture_id = int(mesh.find("Texture").text)
        new_mat_id = custom_mat_id + 1
        custom_mat_id = custom_mat_id + 1
        materials[new_mat_id] = copy.deepcopy(materials[mat_id])
        materials[new_mat_id].attrib["id"] = str(new_mat_id)
        mat_id = new_mat_id
        materials[mat_id].find("DiffuseReflectance").attrib["tex_id"] = str(texture_id)
        materials[mat_id].find("DiffuseReflectance").attrib["tex_mode"] = str(textures[texture_id].decal_mode)

    new_elem = add_mesh(mat_id, faces, texture_id)

    new_elem.attrib["FromTri"] = str(True)
    if tri.find("Transformations") is None:
        etree.SubElement(new_elem, "Transformations").text = " "
    else:
        new_elem.append(copy.deepcopy(tri.find("Transformations")))

    new_objects.append(new_elem)

for mat in materials:
    converted_root.find("Materials").append(materials[mat])

converted_root.attrib["version"] = "1"

print etree.tostring(converted_root)
