<h3>This project uses the framework from CS488 of University of Waterloo and further dive in the topic of ray tracing.</h3>
<h3>Most features are based on the discussion in textbook: <b>Fundamentals of Computer Graphics</b>, some specific implementations borrow the concept from several papers.</h3>
<br>
<h2>Milestones:</h2>
<br>

<h3><b>014 & 015:</b></h3>
<h4>Optimization-bounding box & multithreading</h4>
<div style="margin-bottom: 3%; display: flex;">
    <div style="width: 50%; float: left;text-align:center;margin-top: 5%" markdown="1">
        <img src="./library/014_hb_ex1.png" alt="drawing" class="center" width="300"/>
        <h4>Mesh of a cow </h4>
    </div>  
    <div style="width: 50%; float: left;" markdown="1">
        <h4> We use a mesh of a cow to test our optimization. The cow consists of 2903 triangles. </h4>
        <table>
            <tr>
                <th></th>
                <th>No Bounding Volume</th>
                <th>Bounding Volume</th>
            </tr>
            <tr>
                <td>No Multithreading</td>
                <td>169 sec</td>
                <td>20.45 sec</td>
                </tr>
            <tr>
                <td>Multithreading</td>
                <td>25.1 sec</td>
                <td>3.68 sec</td>
            </tr>
        </table>
        <h4> Before any optimization, the total excution time is very high and this excution time is always in the same magnitude regardless of the size of the mesh</h4>
        <h4> After using the bounding volume, the excution time is much reduced. However, the efficiency bounding volumn is closely related to the size of primitive in camera pyramid</h4>
        <h4> Multithreading boost the excution time by calculating each pixel with an independent thread. The effect is related to the number of extra thread allowed for the program. My test is running with 8 threads, so appoximately 8 times faster.</h4>
    </div>
</div>
<br>

<div style="margin-bottom: 3%;">
    <h3><b>013:</b></h3>
    <h4> Cool-to-warm shading(Gooch shading). Instead of using shadows, surfaces facing in one direction are shaded with a cool color and in the opposite direction are shaded with a warm color. In our implementation, we still use a light to determine the shading color: surface face to the light is shaded with a warm color.</h4>
    <h4>Concept: Section 10.3.2 from Fundamentals of Computer Graphics</h3>
    <h4>Implementation: <I>Gooch, Amy, et al. “A Non-Photorealistic Lighting Model For Automatic Technical Illustration.” Http://Www.cs.utah.edu/, University of Utah, 24 July 1998, faculty.idc.ac.il/arik/LODSeminar/07Shading/gooch-sg98.pdf.</I> </h3>
    <div style="width: 50%; margin-left:auto; margin-right:auto" markdown="1">
        <img src="./library/013_gooch.png" alt="drawing" width="300"/>
    </div>
    <h4 style="text-align:center">The choice of warm color and cool color along with the ratio in which the materials' kd blend with these colors will affect the effect a lot! They needs to be very carefully adjusted to create a artist shading effect. </h4>
</div>
<br>

<div style="margin-bottom: 3%;">
    <h3><b>012:</b></h3>
    <h4>Feature line drawing. It is part of the artistic shading algorithm: Cool-to-warm shading(Gooch shading). The basic idea is to draw silhouettes when adjacent ray hit different objects or there is a huge surface normal discontinuity within the same object.</h4>
    <h4>Concept: Section 10.3.1 from Fundamentals of Computer Graphics</h4>
    <h4>Implementation: <I>Choudhury, A.N.M, and Steven Parker. Ray Tracing NPR-Style Feature Lines. 1 Aug. 2009, www.sci.utah.edu/~roni/website/research/projects/NPR-lines/NPR-lines.NPAR09.pdf.</I> </h4>
    <div style="display: flex;">
        <div style="width: 50%; float: left; text-align:center;" markdown="1">
            <h4>No feature line</h4>
            <a href="">
                <img src="./library/012_fl_no.png" alt="drawing" width="300">
            </a>
        </div>
        <div style="width: 50%; float: left;text-align:center;" markdown="2">
            <h4>Feature line with width 1px</h4>
            <a href="">
                <img src="./library/012_fl.png" alt="drawing" width="300">
            </a>
            <h4>Already created a good mimic of comic drawing.</h4>
        </div>
    </div>
</div>
<br>

<div style="margin-bottom: 5%;">
    <h3><b>009:</b></h3>
    <h4>Supersampling anti-aliasing (SSAA). Using cone tracing to calculate extra smaller pixel color inside the view cone and shade with the average color. </h4>
    <div style="display: flex;">
        <div style="width: 50%; float: left;text-align:center;" markdown="1">
            <a href="">
                <img src="./library/cone.png" alt="drawing" width="150" style="margin-left: 150;">
            </a>
        </div>
        <div style="width: 50%; float: left;" markdown="1">
            <h4> By specify more extra ray is calculated inside the cone and shade with these rays' average color, we could achieve better image quality. </h4>
            <h4> In this implementation, render generate 2 random within [0,1], with one determines the radius from the center and another one determine the angle from 0. Both the randoms generate with Gaussian sampling(needs improvement!) that guarantees most extra rays is within a close range to the center (try box sampling and tent sampling later).</h4>
        </div>
    </div>
    <div style="display: flex; text-align:center;">
        <div style="width: 50%; float: left;" markdown="1">
            <h4 style="margin-top: 10%;"> No extra ray </h4>
            <a href="">
                <img src="./library/009_aa_s0.png" alt="drawing" width=80%>
            </a>
        </div>
        <div style="width: 50%; float: left;" markdown="2">
            <h4 style="margin-top: 10%;"> 8 extra ray </h4>
            <a href="">
                <img src="./library/009_aa_s8.png" alt="drawing" width=80%>
            </a>
        </div>
    </div>
</div>
<br>

<div style="margin-bottom: 3%;">
    <h3><b>006:</b></h3>
    <h4> Ideal specular reflection. By modify maximum recursion depth, we can spicify how much detail is captured within reflections.</h4>
    <div style="display: flex;">
        <div style="width: 50%; float: left; text-align:center;" markdown="1">
            <h4>Max reflection = 1</h4>
            <a href="">
                <img src="./library/006_reflection_d1.png" alt="drawing" width="300">
            </a>
        </div>
        <div style="width: 50%; float: left; text-align:center;" markdown="2">
            <h4>Max reflection = 4</h4>
            <a href="">
                <img src="./library/006_reflection_d4.png" alt="drawing" width="300">
            </a>
            <h4>From the reflection of red ball on the white ball, we can see another red ball reflection </h4>
        </div>
    </div>
</div>

<div>
    <h3><b>005:</b></h3>
    <h4>Basic ray tracing follows the pseudo code in section 4 of the textbook. The render uses Blinn-Phong Shading and able to cast shadow on objects.</h3>
    <div style="width: 50%; margin-left:auto; margin-right:auto" markdown="2">
        <img src="./library/005_shadow.png" alt="drawing" width="300"/>
    </div>
</div>




