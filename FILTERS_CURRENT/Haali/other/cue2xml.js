function W2(n) {
  var s = n.toFixed(0);
  while (s.length < 2)
    s = "0" + s;
  return s;
}

function AddTextElement(parent, name, val) {
  var e = xml.createElement(name);
  e.appendChild(xml.createTextNode(val));
  parent.appendChild(e);
}

function AddTextElement2(parent, name1, name2, val) {
  var e = xml.createElement(name1);
  AddTextElement(e, name2, val);
  parent.appendChild(e);
}

function AddTextElement3(parent, name1, name2, val2, name3, val3) {
  var e = xml.createElement(name1);
  AddTextElement(e, name2, val2);
  AddTextElement(e, name3, val3);
  parent.appendChild(e);
}

function AddTextElement4(parent, name1, name2, val2, name3, val3, name4, val4) {
  var e = xml.createElement(name1);
  AddTextElement(e, name2, val2);
  AddTextElement(e, name3, val3);
  AddTextElement(e, name4, val4);
  parent.appendChild(e);
}

function AddTextElement5(parent, name0, name1, name2, val2, name3, val3, name4, val4) {
  var e = xml.createElement(name0);
  AddTextElement4(e, name1, name2, val2, name3, val3, name4, val4);
  parent.appendChild(e);
}

var xml = new ActiveXObject("Msxml2.DOMDocument.3.0");
var root = xml.createElement("FileInfo");
xml.appendChild(root);

var fso = new ActiveXObject("Scripting.FileSystemObject");
var inf = fso.OpenTextFile(WScript.Arguments(0));

var Gchaplist = new Array();

function AddCh(ch) {
  ch['uid'] = Gchaplist.length + 1;
  Gchaplist.push(ch);
}

var Rtitle = /^\s*TITLE "([^"]*)"/;
var Rperformer = /^\s*PERFORMER "([^"]*)"/;
var Rtrack = /^\s*TRACK (\d+)/;
var Rindex = /^\s*INDEX (\d+) (\d+):(\d+):(\d+)/;

var Gtitle;
var Gperformer;

var ch;

while (!inf.AtEndOfStream) {
  var l = inf.ReadLine();
  if (l.match(Rtitle)) {
    if (ch)
      ch['title'] = RegExp.$1;
    else
      Gtitle = RegExp.$1;
  } else if (l.match(Rperformer)) {
    if (ch)
      ch['perf'] = RegExp.$1;
    else
      Gperformer = RegExp.$1;
  } else if (l.match(Rtrack)) {
    if (ch)
      AddCh(ch);
    ch = new Array();
    ch['index'] = new Array();
  } else if (l.match(Rindex)) {
    if (ch) {
      var ii = new Array();
      ii['name'] = RegExp.$1;
      ii['time'] = W2(Math.floor(RegExp.$2 / 60)) + ":" + W2(Math.floor(RegExp.$2 % 60)) + ":" + RegExp.$3 + (parseInt(RegExp.$4) * 2352 / (44100*4)).toFixed(3).substr(1);
      ch['index'].push(ii);
    }
  }
}

if (ch)
  AddCh(ch);

inf.Close();

var xchR = xml.createElement("Chapters");
root.appendChild(xchR);
var xed = xml.createElement("EditionEntry");
xchR.appendChild(xed);
var xtags = xml.createElement("Tags");
root.appendChild(xtags);

var uu = Gchaplist.length + 1;

for (var ci in Gchaplist) {
  var c = Gchaplist[ci];

  var xc = xml.createElement("ChapterAtom");

  if (c['title'])
    AddTextElement3(xc, "ChapterDisplay", "ChapterString", c['title'], "ChapterLanguage", "und");

  var t;
  for (var iii in c['index']) {
    var ii = c['index'][iii];

    if (ii['name'] == "01")
      t = ii['time'];
    else if (ii['name'] == "00" && !t)
      t = ii['time'];

    var xz = xml.createElement("ChapterAtom");

    AddTextElement(xz, "ChapterFlagHidden", "1");
    AddTextElement2(xz, "ChapterDisplay", "ChapterString", "INDEX " + ii['name']);
    AddTextElement(xz, "ChapterTimeStart", ii['time']);
    AddTextElement(xz, "ChapterPhysicalEquiv", "10");
    AddTextElement(xz, "ChapterUID", uu);

    xc.appendChild(xz);

    ++uu;
  }

  if (t)
    AddTextElement(xc, "ChapterTimeStart", t);

  AddTextElement(xc, "ChapterUID", c['uid']);

  xed.appendChild(xc);

  var xt = xml.createElement("Tag");
  AddTextElement2(xt, "Targets", "ChapterUID", c['uid']);
  AddTextElement3(xt, "Simple", "Name", "PART_NUMBER", "String", c['uid']);
  if (c['title'])
    AddTextElement4(xt, "Simple", "Name", "TITLE", "String", c['title'], "Language", "und");
  if (c['perf'])
    AddTextElement4(xt, "Simple", "Name", "ARTIST", "String", c['perf'], "Language", "und");

  xtags.appendChild(xt);
}

if (Gperformer)
  AddTextElement5(xtags, "Tag", "Simple", "Name", "ARTIST", "String", Gperformer, "Language", "und");

if (Gtitle) {
  AddTextElement5(xtags, "Tag", "Simple", "Name", "TITLE", "String", Gtitle, "Language", "und");
  AddTextElement2(root, "Info", "Title", Gtitle);
}

xml.save(WScript.Arguments(1));
