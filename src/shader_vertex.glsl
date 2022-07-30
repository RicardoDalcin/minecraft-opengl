#version 330 core

// Atributos de v�rtice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a fun��o BuildTriangle() em "main.cpp".
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 color_coefficients;

// Atributos de v�rtice que ser�o gerados como sa�da ("out") pelo Vertex Shader.
// ** Estes ser�o interpolados pelo rasterizador! ** gerando, assim, valores
// para cada fragmento, os quais ser�o recebidos como entrada pelo Fragment
// Shader. Veja o arquivo "shader_fragment.glsl".
out vec4 cor_interpolada_pelo_rasterizador;

// Matrizes computadas no c�digo C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Vari�vel booleana no c�digo C++ tamb�m enviada para a GPU
uniform bool render_as_black;

void main()
{
    // A vari�vel gl_Position define a posi��o final de cada v�rtice
    // OBRIGATORIAMENTE em "normalized device coordinates" (NDC), onde cada
    // coeficiente est� entre -1 e 1.  (Veja {+NDC2+}).
    //
    // O c�digo em "main.cpp" define os v�rtices dos modelos em coordenadas
    // locais de cada modelo (array model_coefficients). Abaixo, utilizamos
    // opera��es de modelagem, defini��o da c�mera, e proje��o, para computar
    // as coordenadas finais em NDC (vari�vel gl_Position). Ap�s a execu��o
    // deste Vertex Shader, a placa de v�deo (GPU) far� a divis�o por W. Veja
    // slides 41-67 e 69-86 do documento Aula_09_Projecoes.pdf.

    gl_Position = projection * view * model * model_coefficients;

    // Como as vari�veis acima  (tipo vec4) s�o vetores com 4 coeficientes,
    // tamb�m � poss�vel acessar e modificar cada coeficiente de maneira
    // independente. Esses s�o indexados pelos nomes x, y, z, e w (nessa
    // ordem, isto �, 'x' � o primeiro coeficiente, 'y' � o segundo, ...):
    //
    //     gl_Position.x = model_coefficients.x;
    //     gl_Position.y = model_coefficients.y;
    //     gl_Position.z = model_coefficients.z;
    //     gl_Position.w = model_coefficients.w;
    //

    if ( render_as_black )
    {
        // Ignoramos o atributo cor dos v�rtices, colocando a cor final como
        // preta. Utilizamos isto para renderizar as arestas pretas dos cubos.
        cor_interpolada_pelo_rasterizador = vec4(0.0f,0.0f,0.0f,1.0f);
    }
    else
    {
        // Copiamos o atributo cor (de entrada) de cada v�rtice para a vari�vel
        // "cor_interpolada_pelo_rasterizador". Esta vari�vel ser� interpolada pelo
        // rasterizador, gerando valores interpolados para cada fragmento!  Veja o
        // arquivo "shader_fragment.glsl".
        cor_interpolada_pelo_rasterizador = color_coefficients;
    }
}

