using System;
using System.Collections.Generic;
using System.IO;

class Program
{
    static List<Vertex> vertices = new List<Vertex>();
    static List<Vec3> positions = new List<Vec3>();
    static List<Vec3> normals = new List<Vec3>();
    static List<Vec2> uvs = new List<Vec2>();

    static List<ushort> indices = new List<ushort>();

    static void Main(string[] args)
    {
        ReadData();
        WriteData();

    }

    private static void WriteData()
    {
        using (BinaryWriter bw = new BinaryWriter(File.Create("HisBinaryFile.bin")))
        {

            //write file header
            bw.Write((uint)vertices.Count);
            bw.Write((uint)indices.Count);

            //Vertex Data
            //glm::vec3 position;
            //glm::vec4 color;
            //glm::vec3 normal;
            //glm::vec2 uv;

            foreach (Vertex v in vertices)
            {
                bw.Write(v.position.X);
                bw.Write(v.position.Y);
                bw.Write(v.position.Z);

                //Color
                bw.Write((float)randy.NextDouble());
                bw.Write((float)randy.NextDouble());
                bw.Write((float)randy.NextDouble());
                bw.Write((float)randy.NextDouble());

                bw.Write(v.normal.X);
                bw.Write(v.normal.Y);
                bw.Write(v.normal.Z);

                bw.Write(v.uv.X);
                bw.Write(v.uv.Y);
            }

            foreach (ushort s in indices)
                bw.Write(s);

            bw.Close();
        }
    }

    static Random randy = new Random();

    private static void ReadData()
    {
        StreamReader sr = new StreamReader("TeddyBear.obj");
        string line;
        while ((line = sr.ReadLine()) != null)
        {
            string[] lineParts = line.Split();
            InterpretLine(lineParts);
        }
    }

    private static void PrintVertexData(Vertex vert)
    {
        Console.WriteLine("Position: (" + vert.position.X + "," + vert.position.Y + "," + vert.position.Z + ")");
        Console.WriteLine("Normal: (" + vert.normal.X + "," + vert.normal.Y + "," + vert.normal.Z + ")");
        Console.WriteLine("UV: (" + vert.uv.X + "," + vert.uv.Y + ")");
        
    }

    private static void InterpretLine(string[] lineParts)
    {
        switch (lineParts[0])
        {
            case "v":
                positions.Add(new Vec3
                {
                    X = float.Parse(lineParts[1]),
                    Y = float.Parse(lineParts[2]),
                    Z = float.Parse(lineParts[3])
                });
                break;
            case "vt":
                uvs.Add(new Vec2
                {
                    X = float.Parse(lineParts[1]),
                    Y = float.Parse(lineParts[2])
                });
                break;
            case "vn":
                normals.Add(new Vec3
                {
                    X = float.Parse(lineParts[1]),
                    Y = float.Parse(lineParts[2]),
                    Z = float.Parse(lineParts[3])
                });
                break;
            case "f":
                AddFace(lineParts);
                break;
        }
    }

    private static void AddFace(string[] lineParts)
    {
        AddAVert(lineParts[1].Split('/'));
        AddAVert(lineParts[2].Split('/'));
        AddAVert(lineParts[3].Split('/'));
    }

    private static void AddAVert(string[] vertParts)
    {
        string[] vertIndices = vertParts;
        Vertex meVert = MakeAVert(vertIndices);
        if (!vertices.Contains(meVert))
            vertices.Add(meVert);

        indices.Add((ushort)vertices.IndexOf(meVert));
    }

    private static Vertex MakeAVert(string[] vertIndices)
    {
        Vertex meVert;

        meVert.position = positions[int.Parse(vertIndices[0]) -1];
        meVert.uv = uvs[int.Parse(vertIndices[1]) -1];
        meVert.normal = normals[int.Parse(vertIndices[2])-1];
        //meVert.tangent =

        return meVert;
    }

    private static void calculateTangetArray()
    {
        Vec3[] tan1 = new Vec3[vertices.Count * 2];
        //Vec3[] tan2 = tan1 + vertices.Count;
        //memset(tan1, 0, vertices.Count * sizeof(Vec3) * 2);

        int triangleCount = indices.Count / 3;

        for (int a = 0; a < triangleCount; a++)
        {
            int triBase = a * 3;
            int i1 = indices[triBase + 0];
            int i2 = indices[triBase + 1];
            int i3 = indices[triBase + 2];

            Vertex v1 = vertices[i1]; 
            Vertex v2 = vertices[i2]; 
            Vertex v3 = vertices[i3];

            float x1 = v2.position.X - v1.position.X;
            float x2 = v3.position.X - v1.position.X;
            float y1 = v2.position.Y - v1.position.Y;
            float y2 = v3.position.Y - v1.position.Y;
            float z1 = v2.position.Z - v1.position.Z;
            float z2 = v3.position.Z - v1.position.Z;

            float s1 = v2.uv.X - v1.uv.X;
            float s2 = v3.uv.X - v1.uv.X;
            float t1 = v2.uv.Y - v1.uv.Y;
            float t2 = v3.uv.Y - v1.uv.Y;

            float r = 1.0F / (s1 * t2 - s2 * t1);
            Vec3 sdir;
            sdir.X = ((t2 * x1 - t1 * x2) * r);
            sdir.Y = ((t2 * y1 - t1 * y2) * r);
            sdir.Z = ((t2 * z1 - t1 * z2) * r);

            //Vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
            //(s1 * z2 - s2 * z1) * r);

            tan1[i1] += sdir;
            tan1[i2] += sdir;
            tan1[i3] += sdir;

            //tan2[i1] += tdir;
            //tan2[i2] += tdir;
            //tan2[i3] += tdir;
        }

        for (int a = 0; a < vertices.Count; a++)
        {
            Vertex vert = vertices[a];
            Vec3 n = vert.normal;
            Vec3 t = tan1[a];

            // Gram-Schmidt orthogonalize
            vert.tangent = Vec3.normalize(t - n * dot(n, t));

            // Calculate handedness
            /////vert.tangent.w = (glm::dot(glm::cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
        }

        Array.Clear(tan1, 0, tan1.Length);
    }

    private static float dot(Vec3 val1, Vec3 val2)
    {
        float ret = 0.0f;
        return ret;
    }
}

struct Vec3
{
    public float X;
    public float Y;
    public float Z;

    public static Vec3 operator+(Vec3 val1, Vec3 val2)
    {
        Vec3 temp;
        temp.X = val1.X + val2.X;
        temp.Y = val1.Y + val2.Y;
        temp.Z = val1.Z + val2.Z;
        return temp;
    }

    public static Vec3 operator -(Vec3 val1, Vec3 val2)
    {
        Vec3 temp;
        temp.X = val1.X - val2.X;
        temp.Y = val1.Y - val2.Y;
        temp.Z = val1.Z - val2.Z;
        return temp;
    }

    public static Vec3 operator*(Vec3 val1, float val2)
    {
        Vec3 ret;
        ret.X = val1.X * val2;
        ret.Y = val1.Y * val2;
        ret.Z = val1.Z* val2;
        return ret;
    }

    public static Vec3 operator/(Vec3 val1, float val2)
    {
        Vec3 ret;
        ret.X = val1.X * val2;
        ret.Y = val1.Y * val2;
        ret.Z = val1.Z * val2;
        return ret;
    }

    private static float Length(Vec3 val)
    {
        double x = (double)val.X;
        double y = (double)val.Y;
        double z = (double)val.Z;

        float ret = (float)Math.Sqrt(x * x + y * y + z * z);
        return ret;
    }

    public static Vec3 normalize(Vec3 val)
    {
        Vec3 ret;
        float length = Length(val);
        if (length != 0)
        {
            ret.X = val.X * (1 / length);
            ret.Y = val.Y * (1 / length);
            ret.Z = val.Z * (1 / length);
        }
        else
        {
            ret.X = 0.0f;
            ret.Y = 0.0f;
            ret.Z = 0.0f;

        }
        return ret;
    }
    public static float dot(Vec3 val1, Vec3 val2)
    {
        float ret = 0.0f;
        return ret;
    }
}

struct Vec2
{
    public float X;
    public float Y;
}

struct Vertex
{
    public Vec3 position;
    public Vec3 tangent;
    public Vec3 normal;
    public Vec2 uv;   
}

/*
 * string[] lineParts = line.Split();
            switch (lineParts[0])
            {
                case "v":
                    positions.Add(new Vec3
                    {
                        X = float.Parse(lineParts[1]),
                        Y = float.Parse(lineParts[2]),
                        Z = float.Parse(lineParts[3])
                    });
                    break;
                case "vt":
                    uvs.Add(new Vec2
                    {
                        X = float.Parse(lineParts[1]),
                        Y = float.Parse(lineParts[2])
                    });
                    break;
                case "vn":
                    normals.Add(new Vec3
                    {
                        X = float.Parse(lineParts[1]),
                        Y = float.Parse(lineParts[2]),
                        Z = float.Parse(lineParts[3])
                    });
                    break;
                case "f":
                    AddFace(lineParts);
                    break;
            }
*/