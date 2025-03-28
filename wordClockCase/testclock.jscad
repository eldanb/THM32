const modeling = require('@jscad/modeling');
const letterFont = require('./engHelvFill');

const letters = [
  "LITASTHISPMAHAVE",
  "ACFIFTEENDOORFUN",
  "TWENTYWFIVEXUIAT",
  "FTHIRTYOSTENSTHE",
  "MINUTESETOURMALL",
  "ORUPASTRTFOURFLY",
  "SEVENOONMIDNIGHT",
  "NINEFIVECTWORZOO",
  "EIGHTFELEVENPARK",
  "SIXTHREEONEGGSKI",
  "TENSEZOCLOCKTOK?",
  "DITAISRAINYNTIME",
  "WINDYBSUNNYRKFOR",
  "STORMYRIKCDINNER",
  "CHILLYFWARMKTOMU",
  "GONLUNCHLBREAKDS"];

// const letters = [
//   "EFGH",
//   "IJKL",
//   "MNOP",
//   "QRSA",
// ];

// z=0 is middle of letter board

const numCols = letters[0].length;
const numRows = letters.length;
const cellSize = 10;

const fontSize = 4.2;
const letterWidth = 0.8;
const letterDepth = 2;

const scheduleSeparatorThickness = 1.5;
const scheduleDepthCol = 7;
const scheduleDepthRow = scheduleDepthCol - 1;

const diffuserThickness = 1;

const frameThickness = 3;
const frameDepth = 18;

const hatchWidth = 20;
const hatchHeight = 10;
const hatchThickness = 1.5;
const hatchSpace = 1;

const coverInset = 2;
const bulgeHeight = 50;
const bulgeDepth = 50;


function extrudedLetter(letter, fontSize, thickness, height) {
  const vectorChar = modeling.text.vectorChar({ height: fontSize, }, letter);
  const allSegments = vectorChar.segments.flatMap(segmentList =>
    segmentList.slice(1).map((segmentPoint2, index) => {
      const [x1, y1] = segmentList[index];
      const [x2, y2] = segmentPoint2;

      const segLen = Math.sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
      const segMid = [(x1 + x2) / 2, (y1 + y2) / 2];
      const segAngle = Math.atan2(y1 - y2, x1 - x2);
      return modeling.transforms.translate(segMid,
        modeling.transforms.rotateZ(segAngle,
          modeling.primitives.rectangle({ size: [segLen, thickness] })
        )
      );
    })
  );

  const baseGeometry = modeling.extrusions.extrudeLinear({ height },
    modeling.booleans.union(...allSegments));
  const center = modeling.measurements.measureCenter(baseGeometry);

  return modeling.transforms.translate([-center[0], -center[1], -center[2]],
    modeling.extrusions.extrudeLinear({ height },
      modeling.booleans.union(...allSegments)));
}


function extrudedLetter2(letter, fontSize, thickness, height) {
  const vectorChar = modeling.text.vectorChar({ height: fontSize, }, letter);
  const extrudedLetter = modeling.booleans.union(
    ...vectorChar.segments.map(segment =>
      modeling.extrusions.extrudeRectangular(
        { height: height, corners: 'round', size: thickness },
        modeling.geometries.path2.fromPoints({ closed: false }, segment))
    )
  );

  const center = modeling.measurements.measureCenter(extrudedLetter);

  return modeling.transforms.translate([-center[0], -center[1], -center[2]],
    extrudedLetter);
}

function letterBoard() {
  const boardWidth = numCols * cellSize;
  const boardHeight = numRows * cellSize;

  return modeling.booleans.subtract(
    modeling.primitives.cuboid({ size: [boardWidth, boardHeight, letterDepth] }),
    ...letters.flatMap((row, rowIndex) =>
      row.split('').map((character, colIndex) =>
        modeling.transforms.translate([
          - boardWidth / 2 + (colIndex + 0.5) * cellSize,
          boardHeight / 2 - (rowIndex + 0.5) * cellSize,
          0],
          extrudedLetter2(character, fontSize, letterWidth, letterDepth * 2))
      )
    )
  );
}

function letterSchedules() {
  const boardWidth = numCols * cellSize;
  const boardHeight = numRows * cellSize;

  const shapes = [];

  for (let col = 0; col < numCols - 1; col++) {
    shapes.push(
      modeling.primitives.cuboid({
        size: [scheduleSeparatorThickness, boardHeight, scheduleDepthCol],
        center: [-boardWidth / 2 + (col + 1) * cellSize, 0, -scheduleDepthCol / 2],
      }));
  }

  for (let row = 0; row < numRows - 1; row++) {
    shapes.push(
      modeling.primitives.cuboid({
        size: [boardWidth, scheduleSeparatorThickness, scheduleDepthRow],
        center: [0, -boardHeight / 2 + (row + 1) * cellSize, -scheduleDepthRow / 2],
      }));
  }

  return modeling.booleans.union(...shapes);
}

function frame(width, height, depth, thickness) {

  return modeling.booleans.subtract(
    modeling.primitives.roundedCuboid({
      roundRadius: 3,
      size: [
        width + thickness * 2,
        height + thickness * 2,
        depth],
    }),
    modeling.primitives.cuboid({
      size: [
        width,
        height,
        depth],
    }),
  );
}

function diffuser() {
  const boardWidth = numCols * cellSize;
  const boardHeight = numRows * cellSize;

  return modeling.primitives.cuboid(
    {
      size: [boardWidth, boardHeight, diffuserThickness],
      center: [0, 0, - letterDepth / 2 - diffuserThickness / 2]
    }
  );
}


function watchOuterFrame() {
  const hatchZ = hatchThickness / 2 + letterDepth / 2 - frameDepth + coverInset + hatchSpace;

  return modeling.booleans.union(
    // Frame with cover subtracted
    modeling.booleans.subtract(
      modeling.transforms.translate([0, 0, - frameDepth / 2 + letterDepth / 2],
        frame(
          numCols * cellSize,
          numRows * cellSize,
          frameDepth,
          frameThickness)),

      watchBackCover()
    ),

    // Top hatch
    modeling.primitives.cuboid({
      size: [
        hatchWidth,
        hatchHeight,
        hatchThickness],

      center: [
        0,
        (numRows * cellSize - hatchHeight) / 2,
        hatchZ
      ]
    }),

    // left Hatch
    modeling.primitives.cuboid({
      size: [
        hatchHeight,
        hatchWidth,
        hatchThickness],

      center: [
        (numCols * cellSize - hatchHeight) / 2,
        0,
        hatchZ
      ]
    }),

    // right Hatch
    modeling.primitives.cuboid({
      size: [
        hatchHeight,
        hatchWidth,
        hatchThickness],

      center: [
        - (numCols * cellSize - hatchHeight) / 2,
        0,
        hatchZ
      ]
    })
  );
}

function watchBackCover() {
  return modeling.booleans.subtract(
    modeling.booleans.union(
      // Cover back
      modeling.transforms.translate([
        0,
        0,
        - coverInset / 2 + letterDepth / 2 - frameDepth + coverInset],
        modeling.primitives.cuboid({
          size: [numCols * cellSize, numRows * cellSize, coverInset]
        })),

      // Cover bulge
      modeling.transforms.translate([0,
        - (numRows * cellSize - bulgeHeight) / 2,
        -bulgeDepth / 2 + letterDepth / 2 - frameDepth + coverInset + 3],

        modeling.primitives.roundedCuboid({
          size: [
            numCols * cellSize + frameThickness * 2,
            bulgeHeight + frameThickness * 2,
            bulgeDepth],
          roundRadius: 3
        })),
    ),

    // Trim back
    modeling.transforms.translate([
      0,
      0,
      - coverInset / 2 + letterDepth / 2 - frameDepth + 2 * coverInset + 1],
      modeling.primitives.cuboid({
        size: [
          numCols * cellSize + 2 * frameThickness,
          numRows * cellSize + 2 * frameThickness,
          coverInset + 2]
      })),

    // Bulge cavity
    modeling.transforms.translate([0,
      - (numRows * cellSize - bulgeHeight) / 2,
      -bulgeDepth / 2 + letterDepth / 2 - frameDepth + coverInset + 3 + frameThickness],

      modeling.primitives.cuboid({
        size: [
          numCols * cellSize,
          bulgeHeight,
          bulgeDepth],
        roundRadius: 3
      })),

    // Bulge hole
    modeling.transforms.translate([
      (numCols * cellSize) / 2 - 15,
      - (numRows * cellSize) / 2 + 15,
      - bulgeDepth],

      modeling.primitives.cylinder({
        radius: 8, height: bulgeDepth / 2
      }))
  );
}

function main() {

  return [
    modeling.booleans.union(
      letterBoard(),
      letterSchedules(),
      watchOuterFrame()
    ),

    modeling.colors.colorize(modeling.colors.cssColors.white,
      diffuser()),

    modeling.transforms.translate([0, 0, -20], watchBackCover())
  ];
}
main();
module.exports = { main };
