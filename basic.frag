#version 330 core

in vec4 channelCol; //Kanal iz Verteks sejdera - mora biti ISTOG IMENA I TIPA kao u vertex sejderu
in vec3 colOffset; //Promena boje u odnosu na udaljenost od x ose
out vec4 outCol; //Izlazni kanal koji ce biti zavrsna boja tjemena (ukoliko se ispisuju podaci u memoriju, koristiti layout kao za ulaze verteks sejdera)

void main()
{
	//outCol = vec4(channelCol.r+colOffset.r,channelCol.g+colOffset.g,channelCol.b+colOffset.b,1.0);
	outCol = channelCol;
}