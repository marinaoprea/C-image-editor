// Copyright Marina Oprea 313CAb 2022-2023

<h3>Overview</h3>
	<p>The current source proposes a solution for a somewhat simple image editor
working with ppm and pgm image formats. The program's behaviour is determined
by the user's commands, read from standard input under specific format.</p>
	<p>The main function reads the input line by line, identifies the given
command and afterwards calls the specific function. It stops the execution when
EXIT command is found.</p>
	<p>Moreover, it calls for the function which initializes the filter structure
to the specific filters used in this approach.</p><br>

<h3>Commands</h3>
<b><i>LOAD</b></i> command:
	the image file is opened and the image type is identified. Then, the pixels'
values are stored in a matrix which is dinamically allocated for ease of passing
it as function parameter.
	Depending on the file type, the input is treated as either <i>ascii code values</i>
or <i>binary code</i>. Depending on the image type, <i>black&white</i>, <i>grayscale</i> or <i>colored</i>,
the matrix type is either simple unsigned char or defined structure to contain
all three values of a pixel, R, G and B.
	In case an image is already loaded, the occupied resources are freed without
the current image version being saved.<br><br>

<b><i>SAVE</b></i> command:
	based on the user's choice, the current image version is saved in a file
either in ascii mode or in binary mode. Based on the image type, the saved 
file fits the ppm or pgm standards. Save command <b>does not</b> deallocate resources.<br><br>

<b><i>SELECT ALL</b></i> command:
	this command updates current image selection to one of the full image. After
being loaded, and image is automatically fully selected and after being cropped,
the cropped version becomes automatically fully selected.<br><br>

<b><i>SELECT</b></i> command:
	this command updates the selection to the one based on the user's choice.<br><br>

<b><i>APPLY</b></i> command:
	this command applies one of the four filters integrated in this programm on
the current image selection, based on user's choice.<br>
	It allocates an auxiliar copy of the selection, calculates new pixel values,
copies them to the initial matrix and deallocates used resources.<br>
	The auxiliar matrix prevents image kernel applications from interfering on
adjacent pixels.<br><br>

<b><i>CROP</b></i> command:
	this command crops the image to the current selection. It allocates a new
matrix of updated dimensions, deallocates the old one and updates the old
matrix's address to the the newly allocated one.<br><br>

<b><i>HISTOGRAM</b></i> command:
	prints the histogram of a grayscale image using specific number of bins and
specific number of stars ("\*"). The pixels' adjacent values are split in bins;
their frequency in the image is represented by printed characters, "\*" number
being proportionate to the maximum number of stars given by the user. <br><br>

<b><i>EQUALIZE</b></i> command:
	equalizes the image's histogram, bringing pixels' frequencies to a common
value. Applied on grayscale images, it repairs poor contrast. <br><br>

<b><i>ROTATE</b></i> command:
	rotates the full image, or a square selection of it. Supported angles are
0, +-90, +-180, +-270, +-360.<br>
	Solution allocates a new matrix for the selected part for rotation,
transposes the values accordingly, then coppies them to the old matrix and
deallocates resources.<br><br>

<b><i>EXIT</b></i> command:
	deallocates all resources and ends program execution.<br>
