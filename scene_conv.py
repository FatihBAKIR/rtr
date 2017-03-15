import sys
import copy
import xml.etree.ElementTree as etree

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
    converted_cameras.append(copy.deepcopy(cam))

converted_root.append(copy.deepcopy(root.find("BackgroundColor")))
converted_root.append(copy.deepcopy(root.find("Lights"))) # lights need no modification

if not (root.find("Transformations") is None):
    converted_root.append(copy.deepcopy(root.find("Transformations"))) # lights need no modification
else:
    etree.SubElement(converted_root, "Transformations")

converted_root.append(copy.deepcopy(root.find("Materials"))) # materials are almost the same
for mat in converted_root.find("Materials").iterfind("Material"):
    if not "shader" in mat.attrib:
        mat.attrib["shader"] = "ceng795" # just set the shaders

def group(lst, n):
    return zip(*[lst[i::n] for i in range(n)])

vertexdata_text = root.find("VertexData").text
vertex_pos = [float(x) for x in vertexdata_text.split()]
vertices = group(vertex_pos, 3)

buffers = etree.SubElement(converted_root, "Buffers")

next_v_buf_id = 0
next_i_buf_id = 0

scn_max = map(max, zip(*vertices))
scn_min = map(min, zip(*vertices))

scn_center =[sum(x) / 2 for x in zip(scn_min, scn_max)]
scn_extent =[max(0.01, y - x) for (x, y) in zip(scn_min, scn_max)]

max_radius = 0

def add_mesh_data(v_buffer, i_buffer):
    vbuf = etree.SubElement(buffers, "VertexBuffer")
    ibuf = etree.SubElement(buffers, "IndexBuffer")

    v_buffer_txt = "\n".join(map(lambda x : " ".join(map(str, x)), v_buffer))
    i_buffer_txt = "\n".join(map(lambda x : " ".join(map(str, x)), i_buffer))

    global next_v_buf_id
    global next_i_buf_id
    v_buf_id = next_v_buf_id
    i_buf_id = next_i_buf_id
    vbuf.attrib["id"] = str(next_v_buf_id)
    next_v_buf_id += 1
    ibuf.attrib["id"] = str(next_i_buf_id)
    next_i_buf_id += 1

    vbuf.text = v_buffer_txt
    ibuf.text = i_buffer_txt

    return (v_buf_id, i_buf_id)

# parse objects

objects = root.find("Objects")
new_objects = etree.SubElement(converted_root, "Objects")

for sphere in objects.iterfind("Sphere"):
    new_elem = copy.deepcopy(sphere)
    max_radius = max(max_radius, float(new_elem.find("Radius").text))
    new_elem.find("Center").text = " ".join(map(str, vertices[int(sphere.find("Center").text) - 1]))
    if (new_elem.find("Transformations") is None):
        etree.SubElement(new_elem, "Transformations")
    new_objects.append(new_elem)

scn_extent = [sum(x) for x in zip(scn_extent, (max_radius, max_radius, max_radius))]

converted_root.attrib["center"] = " ".join(map(str, scn_center))
converted_root.attrib["extent"] = " ".join(map(str, scn_extent))

def add_mesh(mat_id, faces):
    new_elem = etree.Element("Mesh")
    etree.SubElement(new_elem, "Material").text = str(mat_id)

    face_indices = [int(x) for x in faces.text.split()]
    faces = group(face_indices, 3)

    used_vertices = []
    mapping = {}

    indices = set(face_indices)

    for index in indices:
        mapping[index] = len(used_vertices)
        used_vertices.append(vertices[index - 1])

    faces = map(lambda (a, b, c): (mapping[a], mapping[b], mapping[c]), faces)
    v_id, i_id = add_mesh_data(used_vertices, faces)

    etree.SubElement(new_elem, "VertexBuffer").attrib["id"] = str(v_id)
    etree.SubElement(new_elem, "IndexBuffer").attrib["id"] = str(i_id)
    return new_elem

meshes = {}

for mesh in objects.iterfind("Mesh"):
    faces = mesh.find("Faces")
    new_elem = add_mesh(int(mesh.find("Material").text), faces)
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
    new_elem = add_mesh(int(mesh.find("Material").text), faces)

    new_elem.attrib["FromTri"] = str(True)
    if tri.find("Transformations") is None:
        etree.SubElement(new_elem, "Transformations").text = " "
    else:
        new_elem.append(copy.deepcopy(tri.find("Transformations")))

    new_objects.append(new_elem)

converted_root.attrib["version"] = "1"

print etree.tostring(converted_root)
